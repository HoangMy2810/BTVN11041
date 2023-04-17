#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>

#define MAX_LENGTH 1024

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: %s <server-IP-address> <port>\n", argv[0]);
        return 1;
    }
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons(atoi(argv[2]));
    int client = socket(AF_INET, SOCK_STREAM, 0);
    if (client == -1)
    {
        perror("socket() failed");
        return 1;
    }
    if (connect(client, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("connect() failed");
        return 1;
    }
    printf("Connection to %s %s port [tcp/*] succeeded!\n", argv[1], argv[2]);

    while (1)
    {
        char ID[MAX_LENGTH], name[MAX_LENGTH], DOB[MAX_LENGTH], mark[MAX_LENGTH];
        memset(ID, 0, MAX_LENGTH);
        memset(name, 0, MAX_LENGTH);
        memset(DOB, 0, MAX_LENGTH);
        memset(mark, 0, MAX_LENGTH);

        printf("Enter student information:\n");
        printf("\t- ID: ");
        fgets(ID, MAX_LENGTH, stdin);
        ID[strcspn(ID, "\n")] = 0;

        printf("\t- Name: ");
        fgets(name, MAX_LENGTH, stdin);
        name[strcspn(name, "\n")] = 0;

        printf("\t- DOB: ");
        fgets(DOB, MAX_LENGTH, stdin);
        DOB[strcspn(DOB, "\n")] = 0;

        printf("\t- Mark: ");
        fgets(mark, MAX_LENGTH, stdin);
        mark[strcspn(mark, "\n")] = 0;

        // Đóng gói thông tin sinh viên vào buffer để gửi đến server
        char buffer[4 * MAX_LENGTH + 1];
        memset(buffer, 0, 4 * MAX_LENGTH + 1);
        sprintf(buffer, "%s %s %s %s", ID, name, DOB, mark);

        // Gửi buffer chứa thông tin sinh viên đến server
        int bytes_sent = send(client, buffer, strlen(buffer), 0);
        if (bytes_sent == -1)
        {
            perror("send() failed");
            return 1;
        }
        printf("Send successfully\n\n");
        printf("Do you want to continue? (Y/N): ");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = 0;
        if (strcmp(buffer, "N") == 0)
        {
            if (send(client, "exit\n", 5, 0) == -1)
            {
                perror("send() failed");
                return 1;
            }
            break;
        }
    }
    close(client);
    return 0;
}