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

void error_handling(char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_in serv_addr;
    char msg[1024] = {0};
    int str_len;

    cJSON *json;
    cJSON *type;
    const cJSON *sensor_type;
    const cJSON *action;

    if (argc != 3) {
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1)
        error_handling("socket() error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("connect() error");

    printf("Connection established\n");

    while (1) {
        str_len = read(sock, msg, sizeof(msg) - 1);
        if (str_len == -1)
            error_handling("read() error");

        msg[str_len] = '\0';

        json = cJSON_Parse(msg);
        if (json == NULL) {
            fprintf(stderr, "Failed to parse JSON\n");
            continue;
        }

        type = cJSON_GetObjectItem(json, "type");
        if (cJSON_IsString(type) && (type->valuestring != NULL)) {
            if (strcmp(type->valuestring, "sensor") == 0) {
                sensor_type = cJSON_GetObjectItem(json, "sensor_type");
                if (cJSON_IsString(sensor_type) && (sensor_type->valuestring != NULL)) {
                    if (strcmp(sensor_type->valuestring, "humidity") == 0) {
                        action = cJSON_GetObjectItem(json, "value");
                        if (cJSON_IsNumber(action)) {
                            printf("received : %d\n", action->valueint);
                        }
                    }
                }
            }
        }

        cJSON_Delete(json);
        memset(msg, 0, sizeof(msg)); // 버퍼 초기화
    }

    close(sock);

    return 0;
}

