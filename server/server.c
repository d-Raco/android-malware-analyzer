#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>

/* gcc -Wall -g server.c -o server */

/*
Usage:
    First execute this file with the internal IP of the server. (IP is found in the command prompt: ubuntu@ip-172-31-25-98).
    Then execute the python script.
    Then execute the client and make it connect to the server via the external server IP and port 1234. (IP is found in the ssh command: ubuntu@ec2-18-234-209-12.compute-1.amazonaws.com).
*/

#define MAX 1024

char key_items[] = "location,gps,camera,microphone,sound,recorder,telephon,bluetooth,wifi,network,messag,mms,sms,sdcard,storage,contact,nfc,mail,account";


int getResult(int socktcp, char host[NI_MAXHOST]) {
    DIR *FD, *FD_in;
    struct dirent *in_dir, *in_file;
    FILE    *entry_file;
    char    buffer[200], name_dir[50], name_file[100], result[100], substr[50], temp[200], ip[NI_MAXHOST];
    int buffer_length;
    char *pch, *tmp;
    int cnt;
    
    //Result/-XXX/part-000XX (no mirar los _SUCCESS)
    /* Scanning the Result directory */
    if (NULL == (FD = opendir("Result"))) {
        fprintf(stderr, "Error : Failed to open Result directory - %s\n", strerror(errno));
        return 1;
    }

    while ((in_dir = readdir(FD))) {
        if (!strcmp (in_dir->d_name, "."))
            continue;
        if (!strcmp (in_dir->d_name, ".."))    
            continue;

        strcpy(name_dir, "Result/");
        strcat(name_dir, in_dir->d_name);

        if (NULL == (FD_in = opendir(name_dir))) {
            fprintf(stderr, "Error: Failed to open %s directory - %s\n", name_dir, strerror(errno));
            return 1;
        }
        
        while ((in_file = readdir(FD_in))) {
            if (in_file->d_name[0] == '.' || in_file->d_name[0] == '_')
                continue;

            strcpy(name_file, name_dir);
            strcat(name_file, "/");
            strcat(name_file, in_file->d_name);

            /* Open directory entry file for common operation */
            entry_file = fopen(name_file, "r+");
            if (entry_file == NULL) {
                fprintf(stderr, "Error : Failed to open entry file %s - %s\n", name_file, strerror(errno));
                return 1;
            }

            while (fgets(buffer, MAX, entry_file) != NULL) {
                cnt = 0;
                strcpy(result, "");
                strcpy(temp, buffer);
                pch = strtok(temp, "'");
                while (pch != NULL) {
                    if (cnt % 2 != 0) {
                        if (cnt == 1)
                            strcpy(ip, pch);
                        else {
                            strcat(result, pch);
                            strcat(result, " ");
                        }
                    }
                    tmp = pch;
                    pch = strtok(NULL, "'");
                    ++cnt;
                }

                memcpy(substr, &tmp[3], strlen(tmp)-5);
                substr[strlen(tmp)-5] = '\0';
                strcat(result, substr);
                strcat(result, "\n");

                if (strncmp(ip, host, strlen(host)) == 0) {
                    if (cnt == 7)
                        send(socktcp, result, strlen(result), 0);
                    buffer_length = strlen(buffer);

                    fseek(entry_file, -strlen(buffer), SEEK_CUR);
                    strcpy(buffer, "");
                    for (int i = 0; i < buffer_length-1; ++i)
                        strcat(buffer, " ");
                    strcat(buffer, "\n");
                    fwrite(buffer, 1, buffer_length, entry_file);
                }
            }

            fclose(entry_file);
        }
    }

    return 0;
}

void receiveData(char host[NI_MAXHOST], int socktcp, int socktcp_local) {
    char keywords[MAX], filter[MAX*2], apps[MAX*4], cpy_array[MAX*2], cpy_array2[MAX], buff[MAX*2], buff_res[MAX*2], buff_send[MAX*2], *tmp, *tmp2;
    int nr_bytes;
    int out = 0, j = 0;

    nr_bytes = recv(socktcp, keywords, MAX, 0);
    keywords[nr_bytes] = '\0';
    nr_bytes = recv(socktcp, filter, MAX*2, 0);
    filter[nr_bytes] = '\0';

    j = 0;
    strcpy(apps, "");
    nr_bytes = recv(socktcp, cpy_array, MAX*2, 0);
    while (cpy_array[0] != '#') {
        j += nr_bytes;
        strcat(apps, cpy_array);
        nr_bytes = recv(socktcp, cpy_array, MAX*2, 0);
    }
    apps[j] = '\0';

    int keywords_empty = strncmp(keywords, "#", 1);
    int filter_empty = strncmp(filter, "#", 1);

    j = 0;
    while(!out && (nr_bytes = recv(socktcp, buff, MAX*2, 0))) {
        for (int i = 0; i < nr_bytes; ++i) {
            if (buff[i] == '\n') {
                buff_res[j++] = '\n';
                buff_res[j] = '\0';

                if (strncmp(buff_res, "q", 1) == 0) {
                    out = 1;
                    sleep(3);
                    getResult(socktcp, host);
                    strcpy(buff, "Q");
                    send(socktcp, buff, strlen(buff), 0);
                }
                else if (strncmp(buff_res, "-", 1) == 0) {
                    //Ignore
                }
                else {
                    int app_found = 0;
                    strcpy(cpy_array, apps);
                    tmp = strtok(cpy_array, ",");

                    while (tmp != NULL && app_found == 0) {
                        if (strstr(buff_res, tmp) != NULL) { //app found
                            app_found = 1;
                            strcpy(buff_send, host);
                            strcat(buff_send, " ");
                            strcat(buff_send, tmp);
                            strcat(buff_send, " ");
                            strcat(buff_send, "-");
                            strcat(buff_send, " ");
                            strcat(buff_send, buff_res);
                            send(socktcp_local, buff_send, strlen(buff_send), 0);

                            if (keywords_empty == 0 && filter_empty == 0) { //keywords and filter empty
                                int key_item_found = 0;
                                strcpy(cpy_array2, key_items);
                                tmp2 = strtok(cpy_array2, ",");

                                while (tmp2 != NULL && key_item_found == 0) {
                                    if (strstr(buff_res, tmp2) != NULL) { //key_item found
                                        key_item_found = 1;
                                        strcpy(buff_send, host);
                                        strcat(buff_send, " ");
                                        strcat(buff_send, tmp);
                                        strcat(buff_send, " ");
                                        strcat(buff_send, tmp2);
                                        strcat(buff_send, " ");
                                        strcat(buff_send, buff_res);
                                        send(socktcp_local, buff_send, strlen(buff_send), 0);
                                    }
                                    tmp2 = strtok(NULL, ",");
                                }
                            }
                            else {
                                if (keywords_empty != 0) { //keywords set
                                    int keyword_found = 0;
                                    strcpy(cpy_array2, keywords);
                                    tmp2 = strtok(cpy_array2, ",");

                                    while (tmp2 != NULL && keyword_found == 0) {
                                        if (strstr(buff_res, tmp2) != NULL) { //keyword found
                                            keyword_found = 1;
                                            strcpy(buff_send, host);
                                            strcat(buff_send, " ");
                                            strcat(buff_send, tmp2);
                                            strcat(buff_send, " ");
                                            strcat(buff_send, tmp);
                                            strcat(buff_send, " ");
                                            strcat(buff_send, buff_res);
                                            send(socktcp_local, buff_send, strlen(buff_send), 0);

                                            strcpy(buff_send, host);
                                            strcat(buff_send, " ");
                                            strcat(buff_send, tmp2);
                                            strcat(buff_send, " - ");
                                            strcat(buff_send, buff_res);
                                            send(socktcp_local, buff_send, strlen(buff_send), 0);
                                        }
                                        tmp2 = strtok(NULL, ",");
                                    }
                                }
                                if (filter_empty != 0) { //filter set
                                    if (strstr(filter, tmp) != NULL) { //filtered app found
                                        int key_item_found = 0;
                                        strcpy(cpy_array2, key_items);
                                        tmp2 = strtok(cpy_array2, ",");

                                        while (tmp2 != NULL && key_item_found == 0) {
                                            if (strstr(buff_res, tmp2) != NULL) { //key_item found
                                                key_item_found = 1;
                                                strcpy(buff_send, host);
                                                strcat(buff_send, " ");
                                                strcat(buff_send, tmp);
                                                strcat(buff_send, " ");
                                                strcat(buff_send, tmp2);
                                                strcat(buff_send, " ");
                                                strcat(buff_send, buff_res);
                                                send(socktcp_local, buff_send, strlen(buff_send), 0);
                                            }
                                            tmp2 = strtok(NULL, ",");
                                        }
                                    }
                                }
                            }
                        }
                        tmp = strtok(NULL, ",");
                    }
                }

                j = 0;
            }
            else {
                buff_res[j++] = tolower(buff[i]);
            }
        }
    }
}

int main(int argc, char **argv) {
    struct addrinfo *res, *res_local;
    struct addrinfo hints;
    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];
    int sock, socktcp, sock_local, socktcp_local;
    struct sockaddr_storage addr;
    socklen_t addrlen = sizeof(addr);
    pid_t pid;

    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_flags = AI_PASSIVE; // Return 0.0.0.0 o ::
    hints.ai_family = AF_UNSPEC; // IPv4 o IPv6
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo("localhost", "9999", &hints, &res_local) < 0) {
        fprintf(stderr, "Error in getaddrinfo(): Name or service unknown\n");
        return -1;
    }

    sock_local = socket(res_local->ai_family, res_local->ai_socktype, 0);
    bind(sock_local, (struct sockaddr *) res_local->ai_addr, res_local->ai_addrlen);
    listen(sock_local, 1);
    socktcp_local = accept(sock_local, (struct sockaddr *) &addr, &addrlen);

    printf("Spark connection established.\n");

    if (getaddrinfo("0.0.0.0", "1234", &hints, &res) < 0) {
        fprintf(stderr, "Error in getaddrinfo(): Name or service unknown\n");
        return -1;
    }

    sock = socket(res->ai_family, res->ai_socktype, 0);
    bind(sock, (struct sockaddr *) res->ai_addr, res->ai_addrlen);
    listen(sock, 5);

    while(1) {
        socktcp = accept(sock, (struct sockaddr *) &addr, &addrlen);
        pid = fork();

        if(pid == 0) {
            close(sock);
            close(sock_local);
            getnameinfo((struct sockaddr *) &addr, addrlen, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);
            printf("[%i] Connection from Host:%s Port:%s\n", getpid(), host, serv);

            receiveData(host, socktcp, socktcp_local);
            
            printf("\n[%i] Connection terminated...\n", getpid());

            close(socktcp);
            return 0;
        }
        else
            close(socktcp);
    }
    close(sock);
    close(sock_local);
    close(socktcp_local);
    return 0;
}