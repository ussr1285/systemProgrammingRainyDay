#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <cjson/cJSON.h>
#include "../header/temperature_and_humidity.h"

void error_handling(char *message){
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

void create_and_send_json(int socket_fd, int value_from_sensor) {
    cJSON *json = cJSON_CreateObject();
    if (json == NULL) {
        printf("Failed to create JSON object\n");
        return;
    }

    // JSON 데이터 추가
    cJSON_AddStringToObject(json, "type", "sensor");
    cJSON_AddStringToObject(json, "sensor_type", "humidity");
    cJSON_AddNumberToObject(json, "value", value_from_sensor);

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

int main(int argc, char *argv[]) {
    int serv_sock, clnt_sock = -1;
    int data_to_send = 0;

    struct sockaddr_in serv_addr, clnt_addr;
    socklen_t clnt_addr_size;

    if (argc != 2) {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1)
        error_handling("socket() error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("bind() error");

    if (listen(serv_sock, 5) == -1)
        error_handling("listen() error");

    if (clnt_sock < 0){
        clnt_addr_size = sizeof(clnt_addr);
        clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size);
        if (clnt_sock == -1)
            error_handling("accept() error");
    }

    printf("Connection established\n");

    while (1){
        data_to_send = t_and_h();
        create_and_send_json(clnt_sock, data_to_send);

        usleep(10000000);
    }

    close(clnt_sock);
    close(serv_sock);

    return 0;
}
