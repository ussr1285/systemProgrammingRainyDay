#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <cjson/cJSON.h>
#include "../header/speaker.h"

void error_handling(char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

int main(int argc, char *argv[]){
    int sock;
    struct sockaddr_in serv_addr;
    char msg[1024] = {0};
    int str_len;

    cJSON *json;
    cJSON *type;
    const cJSON *actuator_type;
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
        str_len = read(sock, msg, sizeof(msg));

        if (str_len == -1)
            error_handling("read() error");
        
        msg[str_len] = '\0';

        json = cJSON_Parse(msg);
        if (json == NULL){
            fprintf(stderr, "Failed ro parse JSON\n");
            close(sock);
            continue;
        }

        type = cJSON_GetObjectItem(json, "type");
        if (cJSON_IsString(type) && (type->valuestring != NULL)) {
            if (strcmp(type->valueString, "actuator") == 0){
                actuator_type = cJSON_GetObjectItem(json, "actuator_type");

                if (cJSON_IsString(actuator_type) && (actuator_type->valuestring != NULL)){
                    if (strcmp(actuator_type->valuestring, "speaker") == 0){
                        action = cJSON_GetObjectItem(json, "action");
                        if (cJSON_IsString(action) && (action->valuestring != NULL)) {
                            if (strcmp(action->valuestring, "1"))
                                buzzer();
                        }
                    }
                }
            }
        }
    }

    close(sock);
    cJSON_Delete(json);

    return 0;
}
