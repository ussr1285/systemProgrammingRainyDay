#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "cjson/cJSON.h"
#include "../../header/speaker.h"
#include "../../header/temperature_and_humidity.h"

void error_handling(char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

void create_and_send_json(int socket_fd, int value_from_sensor, int flag) {
    cJSON *json = cJSON_CreateObject();
    if (json == NULL) {
        printf("Failed to create JSON object\n");
        return;
    }

    // JSON 데이터 추가
    if (flag){
        cJSON_AddStringToObject(json, "type", "sensor");
        cJSON_AddStringToObject(json, "sensor_type", "humidity");
        cJSON_AddNumberToObject(json, "value", value_from_sensor);
    }
    
    else{
        cJSON_AddStringToObject(json, "type", "actuator");
        cJSON_AddStringToObject(json, "actuator_type", "speaker");
    }

    // JSON 객체를 문자열로 변환
    char *json_str = cJSON_PrintUnformatted(json);
    if (json_str == NULL) {
        printf("Failed to print JSON object\n");
        cJSON_Delete(json);
        return;
    }

    // JSON 문자열 전송
    send(socket_fd, json_str, strlen(json_str), 0);
    printf("Sent response: %s\n", json_str);

    // 메모리 해제
    free(json_str);
    cJSON_Delete(json);
}

int main(int argc, char *argv[]){
    int sock;
    struct sockaddr_in serv_addr;
    char msg[1024] = {0};
    int str_len;
    int data_to_send = 0;

    cJSON *json;
    cJSON *type;
    const cJSON *actuator_type;
    const cJSON *action;


    if (argc != 3) {
    printf("Usage : %s <IP> <port>\n", argv[0]);
    exit(1);
    }

    printf("Connection established\n");

    while (1) {
        sock = socket(PF_INET, SOCK_STREAM, 0);
        if (sock == -1)
            error_handling("socket() error");

        memset(&serv_addr, 0, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
        serv_addr.sin_port = htons(atoi(argv[2]));

        if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
            error_handling("connect() error");

        data_to_send = t_and_h();
        create_and_send_json(sock, data_to_send, 1);
        //create_and_send_json(sock, 10);

        usleep(2000000);

        str_len = read(sock, msg, sizeof(msg)-1);

        if (str_len == -1)
            error_handling("read() error");

        msg[str_len] = '\0';

        json = cJSON_Parse(msg);
        if (json == NULL){
            fprintf(stderr, "Failed to parse JSON: %s\n", cJSON_GetErrorPtr());
            continue;
        }

        type = cJSON_GetObjectItem(json, "status");
        if (cJSON_IsString(type) && (type->valuestring != NULL)) {
            if (strcmp(type->valuestring, "ok") != 0)
                return 1;
        }

        cJSON_Delete(json);
        memset(msg, 0, sizeof(msg));
        close(sock);

        usleep(3000000);

        sock = socket(PF_INET, SOCK_STREAM, 0);
        if (sock == -1)
            error_handling("socket() error");

        memset(&serv_addr, 0, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
        serv_addr.sin_port = htons(atoi(argv[2]));

        if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
            error_handling("connect() error");

        create_and_send_json(sock, data_to_send, 0);
        //create_and_send_json(sock, 10);

        usleep(2000000);

        str_len = read(sock, msg, sizeof(msg)-1);

        if (str_len == -1)
            error_handling("read() error");

        msg[str_len] = '\0';

        printf("msg : %s\n", msg);

        json = cJSON_Parse(msg);
        if (json == NULL){
            fprintf(stderr, "Failed to parse JSON: %s\n", cJSON_GetErrorPtr());
            continue;
        }

        type = cJSON_GetObjectItem(json, "action");
        printf("valueint %d\n", type->valueint);
        if (cJSON_IsNumber(type)) {
            if (type->valueint == 1){
                printf("ok\n");

                buzzer();
            }

        }

        cJSON_Delete(json);
        memset(msg, 0, sizeof(msg));
        close(sock);

        usleep(3000000);
    
    }

    return 0;
}
