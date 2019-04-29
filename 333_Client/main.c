#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<sys/wait.h>

#include <arpa/inet.h>
#include <sys/stat.h>
#include <unistd.h>

#include <netdb.h>
#include <sys/socket.h>
#include <assert.h>

#include <pthread.h>
#include <netdb.h>

#include <openssl/md5.h>
#include <curl/curl.h>

//#include <sys/stat.h>

#define DEST_PORT 9219
#define DEST_IP "127.0.0.1"

#define LOCAL_PORT 9595

#define BUFF_SIZE 500
#define HEAD_SIZE 50
#define STAMP_SIZE 20
#define NAME_SIZE 20
#define WEB_SIZE 1000

#define BUFSIZE 1024
#define SIZE 400
#define MSGSIZE sizeof(struct message)
#define HDRSIZE sizeof(struct header)
#define MAXFILE 100

typedef struct Header
{
    unsigned short int type;
    char src[20];
    char dest[20];
    unsigned int length;
    unsigned int id;
}__attribute__((packed))Header;


//Bob
struct __attribute__((__packed__)) header {

    unsigned short type;
    char source[20];
    char destination[20];
    unsigned int length;
    unsigned int message_id;
};

struct __attribute__((__packed__)) message {

    struct header head;
    char data[SIZE];
};

struct __attribute__((__packed__)) filename {

    char filename[20];
    unsigned char MD5[20];
};

typedef struct __attribute__((__pacaked__))profile_format {

    char name[20];
    char ip[20];
    char continent[20];
    char country[20];
    char region[20];
    char city[20];
    double la;
    double lo;
    char language[20];
    char introduction[50];
};

int testpro_h(char *buff);
int testpro_e(char *buff);
int testpro_l(char *buff);
int testpro_c(char *buff, char * data);
int testpro_w(char *buff);
int ReadySave(char * buff);
int ReadySend(char * buff);

int testpro_r();
int testpro_on();
int testpro_off();

int Command(char *data);
int CommandI(char *data);
int Chatting(char *data);

int testpro_Gh();
int testpro_Ge();
int testpro_Gc(char *buff, char * data);
int testpro_Gl(int mode);

int testpro_Sp(char * name);

int testpro_prof();

void * THR_print_message_function(void *ptr);
void * THR_file_transmit_function(void *ptr);


//Bob

struct message *file_transfer(char *source, char *destination, int size);

struct message *file_upload_request(char *source, char *destination, char *filename);

struct message *file_download_request(char *source, char *destination, char *filename);// edited bob 4.11.19

struct message *p2p_transfer_request(char *source, char *destination, char *filename);// edited bob 4.15.19

struct message *request_to_delete_file(char *source, char *destination, struct filename file);

struct message *file_list_request(char *source, char *destination);

struct message *file_upload_success_ACK(char *source, char *destination);

struct message *file_doesnt_exist(char *source, char *destination);

struct message *file_transfer_complete_ack(char *source, char *destination);

int file_upload_handler(struct message *buffer, int sockfd);

int file_transfer_request_handler(struct message *buffer, int sockfd);

int file_list_handler(struct message *buffer);

int transfer_ack_handler(struct message *buffer, int sockfd);

void error(char *msg);

int message_handler(int sockfd, struct message* buf);

unsigned char * MD5_hash_generator(char *file_name);

int Intital_Profile(char * webdata);

int SearchProcess(char * data, char * type, char * out);

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);

void Search_byT(char *Skey);

void Search_byW(char *Skey);

void Search_byU(char *Skey);

int SearchProcess_f(char * data, char * type, double *out);

int sockfd_t;
int sockfd, new_fd;
char name[20] = "client1";
char Dname[20] = "client2";
char Gname[20] = "test_g1";
char filename[20] = { 0 };
char p2pclient[20] = { 0 }; //edited bob 4.15.2019
int mode = 3;
int download_size = 0; //edited bob 4.15.2019

char sh_buf[BUFF_SIZE] = { 0 };

struct sockaddr_in dest_addr;
struct sockaddr_in dest_addr_t;

struct profile_format my_prof;

int tflag = -1;
int dflag = -1; //edited by bob
int pflag = -1; //edited bob 4.15.2019
int ptflag = -1; //edited bob 4.19.2019

char p2pfilename[20] = {0}; //edited bob 4.18.2019

char buf[BUFF_SIZE] = "test hello";
CURL *curl;

struct in_addr inAddr;

int local_sock;
struct sockaddr_in local_cli;


unsigned char md[MD5_DIGEST_LENGTH];//bob


int main() {

    char buf2[BUFF_SIZE] = "test hello";
    int buflen = 0;
    char data[BUFF_SIZE] = { 0 };


    //loop back 9595
    local_sock = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in local;
    local.sin_family = AF_INET;
    local.sin_port = htons(LOCAL_PORT);
    local.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(local_sock, (struct sockaddr*)&local, sizeof(local)) < 0)
    {
        perror("bind");
        return 3;
    }

    printf("Register?(Y/N)\n");
    //gets(data);
    int len = sizeof(local_cli);
    recvfrom(local_sock, data, BUFF_SIZE, 0, (struct sockaddr*)&local_cli, (socklen_t *)&len);

    //char
    int sh_len=0;
    bzero(sh_buf, BUFF_SIZE);
    sh_len = sprintf(sh_buf, "back-end start\n");
    sendto(local_sock, sh_buf, sh_len, 0, (struct sockaddr*)&local_cli, sizeof(local_cli));



    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("socket failed:%d", errno);
    }

    //char host[30] = { "comp112-01.cs.tufts.edu" };
    char host[30]={"localhost"};

    struct hostent *hostinfo = gethostbyname(host);

    inAddr = *((struct in_addr*)*hostinfo->h_addr_list);
    //assert(hostinfo);
    //dest_addr_t.sin_addr = *(struct in_addr*)*hostinfo->h_addr_list;

    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(DEST_PORT);

    dest_addr.sin_addr.s_addr = inet_addr(inet_ntoa(inAddr));
    bzero(&(dest_addr.sin_zero), 8);

    if (connect(sockfd, (struct sockaddr*)&dest_addr, sizeof(struct sockaddr)) == -1)
    {
        printf("connect failed:%d\n", errno);
    }
    else
    {
        printf("connect success\n");
    }




    if (data[0] == 'Y')
    {
        testpro_r();
    }
    else
    {
        testpro_on();
    }

    char web_buf[1000] = { 0 };

    CURL *curl = curl_easy_init();
    if (curl) {
        CURLcode res;
        curl_easy_setopt(curl, CURLOPT_URL, "http://api.ipstack.com/check?access_key=3d2393db36a3a849d4a7623207112568");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &web_buf);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }
    Intital_Profile(web_buf);

    char *message1 = "thread1";
    pthread_t thread1;
    int ret_thrd1 = pthread_create(&thread1, NULL, &THR_print_message_function, (void *)message1);

    char *message2 = "thread2";
    pthread_t thread2;
    int ret_thrd2 = pthread_create(&thread2, NULL, &THR_file_transmit_function, (void *)message2);


    while (1)
    {
        sleep(1);
        printf("\nend========================================================\n");
        printf("\n\n");
        //system("clear");

        printf("\nbegin======================================================\n");
        printf("please enter:\n");
        //gets(data);
        recvfrom(local_sock, data, BUFF_SIZE, 0, (struct sockaddr*)&local_cli, (socklen_t *)&len);

        if (data[0] == '!')
        {
            break;
        }
        else if (data[0] == '@')
        {
            Command(data);
            bzero(buf, BUFF_SIZE);
            bzero(data, BUFF_SIZE);
            continue;
        }


        Chatting(data);
        //testpro_c(buf, data);

        bzero(buf, BUFF_SIZE);
        bzero(data, BUFF_SIZE);
    }

    testpro_off();

    /*
    testpro_e(buf);
    ReadySend(buf);
    send(sockfd, buf, sizeof(Header), 0);
    bzero(buf, BUFF_SIZE);
    */

    close(sockfd);
    return 0;
}

int testpro_h(char *buff)
{
    struct Header temp;
    bzero(&temp, sizeof(struct Header));

    bzero(buff, BUFF_SIZE);

    temp.type = 1;
    temp.length = 0;
    temp.id = 0;
    sprintf(temp.src, "%s", name);
    sprintf(temp.dest, "Server");

    memcpy(buff, &temp, sizeof(struct Header));
}

int testpro_r()
{
    struct Header temp;
    bzero(&temp, sizeof(struct Header));

    bzero(buf, BUFF_SIZE);

    temp.type = 12;
    temp.length = 0;
    temp.id = 0;
    sprintf(temp.src, "%s", name);
    sprintf(temp.dest, "Server");

    memcpy(buf, &temp, HEAD_SIZE);

    ReadySend(buf);
    send(sockfd, buf, HEAD_SIZE, 0);
}

int testpro_on()
{
    struct Header temp;
    bzero(&temp, sizeof(struct Header));

    bzero(buf, BUFF_SIZE);

    temp.type = 10;
    temp.length = 0;
    temp.id = 0;
    sprintf(temp.src, "%s", name);
    sprintf(temp.dest, "Server");

    memcpy(buf, &temp, HEAD_SIZE);

    ReadySend(buf);
    send(sockfd, buf, HEAD_SIZE, 0);
}

int testpro_off()
{
    struct Header temp;
    bzero(&temp, sizeof(struct Header));

    bzero(buf, BUFF_SIZE);

    temp.type = 11;
    temp.length = 0;
    temp.id = 0;
    sprintf(temp.src, "%s", name);
    sprintf(temp.dest, "Server");

    memcpy(buf, &temp, HEAD_SIZE);

    ReadySend(buf);
    send(sockfd, buf, HEAD_SIZE, 0);
}

int testpro_e(char *buff)
{
    struct Header temp;
    bzero(&temp, sizeof(struct Header));

    bzero(buff, BUFF_SIZE);

    temp.type = 6;
    temp.length = 0;
    temp.id = 0;
    sprintf(temp.src, "%s", name);
    sprintf(temp.dest, "Server");

    memcpy(buff, &temp, sizeof(struct Header));
}

int testpro_l(char *buff)
{
    struct Header temp;
    bzero(&temp, sizeof(struct Header));

    bzero(buff, BUFF_SIZE);

    temp.type = 3;
    temp.length = 0;
    temp.id = 0;
    sprintf(temp.src, "%s", name);
    sprintf(temp.dest, "Server");

    memcpy(buff, &temp, sizeof(struct Header));
}

int testpro_c(char *buff, char * data)
{
    //char data[20] = "test! hello!";
    struct Header temp;
    bzero(&temp, sizeof(struct Header));

    bzero(buff, BUFF_SIZE);

    temp.type = 5;
    temp.length = strlen(data);
    temp.id = 1236;
    sprintf(temp.src, "%s", name);
    sprintf(temp.dest, "%s", Dname);

    memcpy(buff, &temp, HEAD_SIZE);
    memcpy(buff + HEAD_SIZE, data, temp.length);

    return 1;
}

int testpro_w(char *buff)
{
    char data[20] = "test! wrong!";
    struct Header temp;
    bzero(&temp, sizeof(struct Header));

    bzero(buff, BUFF_SIZE);

    temp.type = 0;
    temp.length = strlen(data);
    temp.id = 1236;
    sprintf(temp.src, "%s", name);
    sprintf(temp.dest, "%s", Dname);

    memcpy(buff, &temp, HEAD_SIZE);
    memcpy(buff + HEAD_SIZE, data, temp.length);

    return 1;
}


int ReadySend(char * buff)
{
    Header tempH;
    short temps = 0;
    int tempi = 0;

    memcpy(&tempH, buff, HEAD_SIZE);

    temps = htons(tempH.type);
    tempH.type = temps;

    tempi = htonl(tempH.id);
    tempH.id = tempi;

    tempi = htonl(tempH.length);
    tempH.length = tempi;


    bzero(buff, HEAD_SIZE);
    memcpy(buff, &tempH, HEAD_SIZE);
    return 1;
}

//Will change ntoh
int ReadySave(char * buff)
{
    Header tempH;
    short temps = 0;
    int tempi = 0;

    memcpy(&tempH, buff, HEAD_SIZE);

    temps = ntohs(tempH.type);
    tempH.type = temps;

    tempi = ntohl(tempH.id);
    tempH.id = tempi;

    tempi = ntohl(tempH.length);
    tempH.length = tempi;


    bzero(buff, HEAD_SIZE);
    memcpy(buff, &tempH, HEAD_SIZE);
    return 1;
}


void * THR_print_message_function(void *ptr)
{
    char buff[BUFF_SIZE] = { 0 };
    Header temp_H;


    char sh_len = 0;

    char *data = buff + HEAD_SIZE;

    char *time = data;
    char *place = data;
    char *message = data + STAMP_SIZE;
    int i = 0;

    bzero(sh_buf, BUFF_SIZE);
    sh_len = sprintf(sh_buf, "initial success!\n");
    sendto(local_sock, sh_buf, sh_len, 0, (struct sockaddr*)&local_cli, sizeof(local_cli));
    printf("\n");
    while (1)
    {
        bzero(buff, BUFF_SIZE);

        int testr = recv(sockfd, buff, BUFF_SIZE, 0);

        if (testr > 0)
        {
            printf("\n");
            ReadySave(buff);
            memcpy(&temp_H, buff, HEAD_SIZE);



            if (temp_H.type == 8)
            {
                printf("erro8\n");


                bzero(sh_buf, BUFF_SIZE);
                sh_len = sprintf(sh_buf, "erro8\n");
                sendto(local_sock, sh_buf, sh_len, 0, (struct sockaddr*)&local_cli, sizeof(local_cli));
            }
            else if (temp_H.type == 7)
            {
                printf("erro7\n");


                bzero(sh_buf, BUFF_SIZE);
                sh_len = sprintf(sh_buf, "erro7\n");
                sendto(local_sock, sh_buf, sh_len, 0, (struct sockaddr*)&local_cli, sizeof(local_cli));
            }
            else if (temp_H.type == 5)
            {
                printf("At %s\n", time);
                printf("%s say: %s\n", temp_H.src, message);


                bzero(sh_buf, BUFF_SIZE);
                sh_len = sprintf(sh_buf, "\nAt %s\n", time);
                sendto(local_sock, sh_buf, sh_len, 0, (struct sockaddr*)&local_cli, sizeof(local_cli));

                bzero(sh_buf, BUFF_SIZE);
                sh_len = sprintf(sh_buf, "%s say: %s\n", temp_H.src, message);
                sendto(local_sock, sh_buf, sh_len, 0, (struct sockaddr*)&local_cli, sizeof(local_cli));


            }
            else if (temp_H.type == 36)//group chatting
            {
                printf("At GMT: %s\n", time);
                printf("%s in Group %s say: %s\n", message, temp_H.src, message + NAME_SIZE);


                bzero(sh_buf, BUFF_SIZE);
                sh_len = sprintf(sh_buf, "\nAt GMT: %s\n", time);
                sendto(local_sock, sh_buf, sh_len, 0, (struct sockaddr*)&local_cli, sizeof(local_cli));

                bzero(sh_buf, BUFF_SIZE);
                sh_len = sprintf(sh_buf, "%s in Group %s say: %s\n", message, temp_H.src, message + NAME_SIZE);
                sendto(local_sock, sh_buf, sh_len, 0, (struct sockaddr*)&local_cli, sizeof(local_cli));
            }
            else if (temp_H.type == 32)//join group
            {
                printf("\nWelcome to group %s\n", temp_H.src);


                bzero(sh_buf, BUFF_SIZE);
                sh_len = sprintf(sh_buf, "Welcome to group %s\n", temp_H.src);
                sendto(local_sock, sh_buf, sh_len, 0, (struct sockaddr*)&local_cli, sizeof(local_cli));
            }
            else if (temp_H.type == 34)//group list
            {

                printf("Group %s\n", temp_H.src);

                bzero(sh_buf, BUFF_SIZE);
                sh_len = sprintf(sh_buf, "\nGroup %s\n", temp_H.src);
                sendto(local_sock, sh_buf, sh_len, 0, (struct sockaddr*)&local_cli, sizeof(local_cli));

                printf("name list: \n");

                bzero(sh_buf, BUFF_SIZE);
                sh_len = sprintf(sh_buf, "name list: \n");
                sendto(local_sock, sh_buf, sh_len, 0, (struct sockaddr*)&local_cli, sizeof(local_cli));

                for (i = HEAD_SIZE; i < testr; i++)
                {
                    printf("%c", buff[i]);

                    bzero(sh_buf, BUFF_SIZE);
                    sh_len = sprintf(sh_buf, "%c", buff[i]);
                    sendto(local_sock, sh_buf, sh_len, 0, (struct sockaddr*)&local_cli, sizeof(local_cli));
                }
                printf("\n");

                bzero(sh_buf, BUFF_SIZE);
                sh_len = sprintf(sh_buf, "\n");
                sendto(local_sock, sh_buf, sh_len, 0, (struct sockaddr*)&local_cli, sizeof(local_cli));

                time = data;
            }
            else if (temp_H.type == 19)
            {
                tflag = 1;

            }
            else if (temp_H.type == 15)//show profile
            {
                struct profile_format temp_prof;
                //char temp_cmd[100] = { 0 };
                bzero(&temp_prof, sizeof(struct profile_format));
                memcpy(&temp_prof, data + 100, sizeof(struct profile_format));

                printf("User name: %s\n", temp_prof.name);

                bzero(sh_buf, BUFF_SIZE);
                sh_len = sprintf(sh_buf, "\nUser name: %s\n", temp_prof.name);
                sendto(local_sock, sh_buf, sh_len, 0, (struct sockaddr*)&local_cli, sizeof(local_cli));


                printf("From: %s, %s, %s\n", temp_prof.city, temp_prof.region, temp_prof.country);

                bzero(sh_buf, BUFF_SIZE);
                sh_len = sprintf(sh_buf, "From: %s, %s, %s\n", temp_prof.city, temp_prof.region, temp_prof.country);
                sendto(local_sock, sh_buf, sh_len, 0, (struct sockaddr*)&local_cli, sizeof(local_cli));

                printf("Language: %s\n", temp_prof.language);

                bzero(sh_buf, BUFF_SIZE);
                sh_len = sprintf(sh_buf, "Language: %s\n", temp_prof.language);
                sendto(local_sock, sh_buf, sh_len, 0, (struct sockaddr*)&local_cli, sizeof(local_cli));

                printf("Self-Introduction: \n");

                bzero(sh_buf, BUFF_SIZE);
                sh_len = sprintf(sh_buf, "Self-Introduction: \n");
                sendto(local_sock, sh_buf, sh_len, 0, (struct sockaddr*)&local_cli, sizeof(local_cli));

                printf("%s\n", temp_prof.introduction);

                bzero(sh_buf, BUFF_SIZE);
                sh_len = sprintf(sh_buf, "%s\n", temp_prof.introduction);
                sendto(local_sock, sh_buf, sh_len, 0, (struct sockaddr*)&local_cli, sizeof(local_cli));

                printf("Google map html: %s\n", place);

                bzero(sh_buf, BUFF_SIZE);
                sh_len = sprintf(sh_buf, "Google map html: %s\n", place);
                sendto(local_sock, sh_buf, sh_len, 0, (struct sockaddr*)&local_cli, sizeof(local_cli));

                //sprintf(temp_cmd, "explorer %s", place);
                //system(temp_cmd);
            }
                //edited by bob on 4.1.2019.
            else if (temp_H.type == 20)
            {
                //download_size. how to extract download_size from type 20 message.
                //tell me how to read buffer msg length.
                download_size = temp_H.length;

                dflag = 1;
            }
                //edited bob 4.19.2019
            else if(temp_H.type == 16){
                ptflag = 1;
            }

                //edited bob 4.15.2019
            else if(temp_H.type == 17){

                download_size = temp_H.length;
                bzero(p2pfilename, 20);
                memcpy(p2pfilename, buff + 50, 20);

                pflag = 1;
            }
        }
        else if (testr <= 0)
        {
            printf("erro801\n");

            bzero(sh_buf, BUFF_SIZE);
            sh_len = sprintf(sh_buf, "erro801\n");
            sendto(local_sock, sh_buf, sh_len, 0, (struct sockaddr*)&local_cli, sizeof(local_cli));

            sleep(5);
        }
    }
}

void * THR_file_transmit_function(void *ptr)
{
    FILE* fp;
    struct stat st;
    int file_size;
    int z;
    int nbytes;

    char sh_len = 0;

    int p2pt_sock; //edited bob 4.19.2019

    while (1)
    {
        sleep(3);
        if (tflag == 1)
        {
            sockfd_t = socket(AF_INET, SOCK_STREAM, 0);
            if (sockfd_t == -1)
            {
                printf("socket failed:%d", errno);
            }

            //char host[30]={"comp112-01.cs.tufts.edu"};
            dest_addr_t.sin_family = AF_INET;
            dest_addr_t.sin_port = htons(9000);
            dest_addr_t.sin_addr.s_addr = inet_addr(inet_ntoa(inAddr));

            bzero(&(dest_addr_t.sin_zero), 8);

            //            int con_num=0;
            //            while(con_num<=3)
            //            {
            //                con_num++;
            //                sleep(1);
            //                if (connect(sockfd_t, (struct sockaddr*)&dest_addr_t, sizeof(struct sockaddr)) == -1)
            //                {
            //                    printf("connect failed:%d", errno);
            //
            //                    //continue;
            //                }
            //                else
            //                {
            //                    printf("connect success\n");
            //                    break;
            //                }
            //
            //            }
            //
            //            if(con_num>=3)
            //            {
            //                printf("connect total failed:%d", errno);
            //                continue;
            //}


            if (connect(sockfd_t, (struct sockaddr*)&dest_addr_t, sizeof(struct sockaddr)) == -1)
            {
                printf("connect failed:%d\n", errno);
                continue;
            }
            printf("connect success\n");
            //question here: sending file and message on different socket. is this correct??
//            struct message *msg1 = file_upload_success_ACK(buffer->head.destination, buffer->head.source);
//            nbytes = send(sockfd, (char*)msg1, 50, 0);

            fp = fopen(filename, "r");

            fstat(fileno(fp), &st);
            file_size = st.st_size;
            printf("what is file size: %d\n", file_size);

            char * buf_send = (char*)malloc(sizeof(char) * file_size);

            z = fread(buf_send, 1, file_size, fp); //read from file pointer position, read 512 bytes.
            nbytes = send(sockfd_t, buf_send, file_size, 0);

            bzero(sh_buf, BUFF_SIZE);
            sh_len = sprintf(sh_buf, "\nfile upload successful!\n");
            sendto(local_sock, sh_buf, sh_len, 0, (struct sockaddr*)&local_cli, sizeof(local_cli));

            close(sockfd_t);
            tflag = -1;
        }
        //edited by bob on 4.1.2019
        if (dflag == 1)
        {
            printf("processing download request\n");

            sockfd_t = socket(AF_INET, SOCK_STREAM, 0);
            if (sockfd_t == -1)
            {
                printf("socket failed:%d", errno);
            }

            //char host[30]={"comp112-01.cs.tufts.edu"};
            dest_addr_t.sin_family = AF_INET;
            dest_addr_t.sin_port = htons(9002);
            dest_addr_t.sin_addr.s_addr = inet_addr(inet_ntoa(inAddr));

            bzero(&(dest_addr_t.sin_zero), 8);

            //int con_num=0;
            if (connect(sockfd_t, (struct sockaddr*)&dest_addr_t, sizeof(struct sockaddr)) == -1)
            {
                printf("connect failed:%d\n", errno);
                continue;
            }
            printf("connect success\n");


            fp = fopen(filename, "w");

            char *buf_receive = (char *)malloc(sizeof(char) * download_size);

            nbytes = recv(sockfd_t, buf_receive, download_size, 0);
            z = fwrite(buf_receive, 1, download_size, fp);

            bzero(sh_buf, BUFF_SIZE);
            sh_len = sprintf(sh_buf, "\nfile download successful!\n");
            sendto(local_sock, sh_buf, sh_len, 0, (struct sockaddr*)&local_cli, sizeof(local_cli));

            fclose(fp);

            close(sockfd_t);
            dflag = -1;
        }

        //edited bob 4.15.2019
        if(pflag==1)
        {
            printf("processing p2p receiving side transfer, establishing connection\n");

            sockfd_t = socket(AF_INET, SOCK_STREAM, 0);
            if (sockfd_t == -1)
            {
                printf("socket failed:%d", errno);
            }

            //char host[30]={"comp112-01.cs.tufts.edu"};
            dest_addr_t.sin_family = AF_INET;
            dest_addr_t.sin_port = htons(9005);
            dest_addr_t.sin_addr.s_addr = inet_addr(inet_ntoa(inAddr));

            bzero(&(dest_addr_t.sin_zero), 8);

            if (connect(sockfd_t, (struct sockaddr*)&dest_addr_t, sizeof(struct sockaddr)) == -1)
            {
                printf("connect failed:%d", errno);
                continue;
            }
            printf("connect success\n");

            fp = fopen(p2pfilename, "w");

            char *buf_receive = malloc(sizeof(char) * download_size);

            nbytes = recv(sockfd_t, buf_receive, download_size, 0);
            z = fwrite(buf_receive, 1, download_size, fp);

            fclose(fp);
            close(sockfd_t);
            pflag=-1;
        }

        if(ptflag==1)
        {
            printf("processing p2p sending side transfer, establishing connection with: %s\n", p2pclient);
            struct sockaddr_in clientaddr;
            int clientlen = sizeof(clientaddr);

            sockfd_t = socket(AF_INET, SOCK_STREAM, 0);
            if (sockfd_t == -1)
            {
                printf("socket failed:%d", errno);
            }

            //char host[30]={"comp112-01.cs.tufts.edu"};
            dest_addr_t.sin_family = AF_INET;
            dest_addr_t.sin_port = htons(9005);
            dest_addr_t.sin_addr.s_addr = inet_addr(inet_ntoa(inAddr));

            bzero(&(dest_addr_t.sin_zero), 8);

            if (bind(sockfd_t, (struct sockaddr *) &dest_addr_t,
                     sizeof(dest_addr_t)) < 0)
                error("ERROR on binding");

            if (listen(sockfd_t, 1) < 0) {
                perror("listen");
                exit(EXIT_FAILURE);
            }
            else {
                printf("initial connection successful!\n");
            }

            p2pt_sock = accept(sockfd_t, (struct sockaddr *) &clientaddr, &clientlen);

            fp = fopen(p2pfilename, "r");

            fstat(fileno(fp), &st);
            file_size = st.st_size;
            printf("what is file size: %d\n", file_size);

            char * buf_send = (char*)malloc(sizeof(char) * file_size);

            z = fread(buf_send, 1, file_size, fp); //read from file pointer position, read 512 bytes.
            nbytes = send(p2pt_sock, buf_send, file_size, 0);

            sleep(1);
            close(p2pt_sock);

            sleep(1);
            close(sockfd_t);

            ptflag = -1;

        }
    }


}

int Command(char *data)
{
    char command = *(data + 1);
    char command2 = *(data + 2);
    char Skey[20] = { 0 };
    switch (command)
    {
        case 'C':
            if (command2 == 'N')//1
            {
                memcpy(Dname, data + 4, strlen(data + 4));
                printf("Dst name: %s", Dname);
            }

            break;

        case 'F':
            if (command2 == 'R')//up req
            {
                bzero(filename, 20);
                memcpy(filename, data + 4, strlen(data + 4));
                struct message *msg = file_upload_request(name, "Server", filename);

                ReadySend((char *)msg);
                send(sockfd, msg, 90, 0);
            }

            //edited by bob, on 4.1.2019.
            if (command2 == 'D')//download request
            {
                bzero(filename, 20);
                memcpy(filename, data + 4, strlen(data + 4));
                struct message* msg = file_download_request(name, "Server", filename);

                ReadySend((char *)msg);
                send(sockfd, msg, 70, 0);
            }

            //edited bob 4.15.2019
            if  (command2 == 'P')//P2P request, first to server, server to client
            {
                //enter @FP p2p.txt client_name to request to send file to another client.
                bzero(filename, 20);
                memcpy(filename, data + 4, 7);
                bzero(p2pclient, 20);
                memcpy(p2pclient, data + 12, 12); //change this to another get instruction.
                bzero(p2pfilename, 20);
                memcpy(p2pfilename, filename, 20);

                struct message *msg = p2p_transfer_request(name, p2pclient, filename);

                ReadySend(msg);
                send(sockfd, msg, 70, 0);
            }

            break;

        case 'G':
            if (command2 == 'N')//11
            {
                memcpy(Gname, data + 4, strlen(data + 4));
                printf("Group name: %s", Gname);

            }
            else if (command2 == 'J')//12
            {
                bzero(Gname, 20);
                memcpy(Gname, data + 4, strlen(data + 4));
                printf("Group name: %s", Gname);

                testpro_Gh();
            }
            else if (command2 == 'E')//13
            {
                bzero(Gname, 20);
                memcpy(Gname, data + 4, strlen(data + 4));
                printf("Group name: %s", Gname);

                testpro_Ge();
            }
            else if (command2 == 'L')//14
            {
                bzero(Gname, 20);
                memcpy(Gname, data + 4, strlen(data + 4));
                printf("Group name: %s", Gname);

                testpro_Gl(3);
            }

            break;

        case 'R':
            if (command2 == 'L')//14
            {
                printf("Region name: %s", my_prof.region);

                testpro_Gl(2);
            }

            break;

        case 'S':
            if (command2 == 'P')
            {
                testpro_Sp((data + 4));
            }
            else if (command2 == 'W') {
                bzero(Skey, 20);
                memcpy(Skey, data + 4, strlen(data + 4));
                printf("Search by key word: %s", Skey);
                Search_byW(Skey);
            }
            else if (command2 == 'T') {
                bzero(Skey, 20);
                memcpy(Skey, data + 4, strlen(data + 4));
                printf("Search by time: %s", Skey);
                Search_byT(Skey);
            }
            else if (command2 == 'N') {
                bzero(Skey, 20);
                memcpy(Skey, data + 4, strlen(data + 4));
                printf("Search by username: %s", Skey);
                Search_byU(Skey);
            }
            break;

        case 'M'://20
            mode = command2 - '0';
            printf("Mode: %d ", mode);

            if (mode == 0)
            {
                printf("Dst name: %s\n", Dname);
            }
            else if (mode == 3)
            {
                printf("Group name: %s\n", Gname);
            }
            else if (mode == 2)//region
            {
                printf("Region name: %s\n", my_prof.region);
            }

            break;

        default:
            break;
    }
}

int testpro_Gh()
{
    struct Header temp;

    bzero(&temp, sizeof(struct Header));

    bzero(buf, BUFF_SIZE);

    temp.type = 31;
    temp.length = 20;
    temp.id = 0;
    sprintf(temp.src, "%s", name);
    sprintf(temp.dest, "Server");


    memcpy(buf, &temp, HEAD_SIZE);
    memcpy(buf + HEAD_SIZE, &Gname, 20);

    ReadySend(buf);
    send(sockfd, buf, 70, 0);

    return 70;
}

int testpro_Gc(char *buff, char * data)
{
    //char data[20] = "test! hello!";
    struct Header temp;
    bzero(&temp, sizeof(struct Header));

    bzero(buff, BUFF_SIZE);

    temp.type = 35;
    temp.length = strlen(data);
    temp.id = 0;
    sprintf(temp.src, "%s", name);
    sprintf(temp.dest, "%s", Gname);

    memcpy(buff, &temp, HEAD_SIZE);
    memcpy(buff + HEAD_SIZE, data, temp.length);

    return 1;
}

int testpro_Ge()
{
    struct Header temp;

    bzero(&temp, sizeof(struct Header));

    bzero(buf, BUFF_SIZE);

    temp.type = 37;
    temp.length = 20;
    temp.id = 0;
    sprintf(temp.src, "%s", name);
    sprintf(temp.dest, "Server");


    memcpy(buf, &temp, HEAD_SIZE);
    memcpy(buf + HEAD_SIZE, &Gname, 20);

    ReadySend(buf);
    send(sockfd, buf, 70, 0);

    return 70;
}

int testpro_Gl(int mode)
{
    struct Header temp;

    bzero(&temp, sizeof(struct Header));

    bzero(buf, BUFF_SIZE);

    temp.type = 33;
    temp.length = 0;

    sprintf(temp.src, "%s", name);

    if (mode == 2)
    {
        memcpy(temp.dest, my_prof.region, 20);
        temp.id = 2;
    }
    else
    {
        memcpy(temp.dest, Gname, 20);
        temp.id = 0;
    }


    memcpy(buf, &temp, HEAD_SIZE);

    ReadySend(buf);
    send(sockfd, buf, HEAD_SIZE, 0);

    return HEAD_SIZE;
}

int Chatting(char *data)
{
    if (mode == 0)
    {
        testpro_c(buf, data);
        ReadySend(buf);
        send(sockfd, buf, strlen(data) + HEAD_SIZE, 0);
    }
    else if (mode == 3)
    {
        testpro_Gc(buf, data);
        ReadySend(buf);
        send(sockfd, buf, strlen(data) + HEAD_SIZE, 0);
    }
}

int CommandI(char *data)
{
    char command = *(data + 1);
    char command2 = *(data + 2);

    switch (command)
    {
        case 'O':
            if (command2 == 'N')
            {
                testpro_on();
            }
            else if (command2 == 'F')
            {
                testpro_off();
            }
        case 'M'://20

            break;

        default:
            break;
    }
}

//Bob
struct message *file_transfer(char *source, char *destination, int size) {
    struct message *msg = (struct message *)malloc(MSGSIZE);
    msg->head.type = 20;
    strcpy(msg->head.source, source);
    strcpy(msg->head.destination, destination);
    msg->head.length = size;
    msg->head.message_id = 0;
    memset((char *)(msg->data), 0, SIZE);
    return msg;
}

struct message *file_upload_request(char *source, char *destination, char *filename) {
    struct message *msg = (struct message *)malloc(MSGSIZE);
    msg->head.type = 21;
    strcpy(msg->head.source, source);
    strcpy(msg->head.destination, destination);
    msg->head.length = 40;
    msg->head.message_id = 0;
    memset((char *)(msg->data), 0, SIZE);
    memcpy((char *)(msg->data), filename, 20);
    MD5_hash_generator(filename);
    memcpy((char *)(msg->data) + 20, md, 20);
    return msg;
}

struct message *file_download_request(char *source, char *destination, char *filename) { //edited by bob
    struct message *msg = (struct message *)malloc(MSGSIZE);
    msg->head.type = 22;
    strcpy(msg->head.source, source);
    strcpy(msg->head.destination, destination);
    msg->head.length = 20;
    msg->head.message_id = 0;
    memset((char *)(msg->data), 0, SIZE);
    memcpy((char*)(msg->data), filename, 20);
    //we don't have the MD5.
    return msg;
}

struct message *p2p_transfer_request(char *source, char *destination, char *filename) { //edited bob 4.15.2019
    struct message *msg = (struct message *)malloc(MSGSIZE);
    int file_size = 0;
    struct stat st;
    msg->head.type = 18;
    strcpy(msg->head.source, source);
    strcpy(msg->head.destination, destination);

    //get the size of the file to send.
    FILE* temp;
    temp = fopen(filename, "r");

    fstat(fileno(temp), &st);
    file_size = st.st_size;

    fclose(temp);

    msg->head.length = file_size;
    msg->head.message_id = 0;
    memset((char *)(msg->data), 0, SIZE);
    memcpy((char*)(msg->data), filename, 20);
//    MD5_hash_generator(filename);
//    memcpy((char *)(msg->data) + 20, md, 20);
    return msg;
}

struct message *request_to_delete_file(char *source, char *destination, struct filename file) {
    struct message *msg = (struct message *)malloc(MSGSIZE);
    msg->head.type = 23;
    strcpy(msg->head.source, source);
    strcpy(msg->head.destination, destination);
    msg->head.length = 0;
    msg->head.message_id = 0;
    memset((char *)(msg->data), 0, SIZE);
    return msg;
}

struct message *file_list_request(char *source, char *destination) {
    struct message *msg = (struct message *)malloc(MSGSIZE);
    msg->head.type = 24;
    strcpy(msg->head.source, source);
    strcpy(msg->head.destination, destination);
    msg->head.length = 0;
    msg->head.message_id = 0;
    memset((char *)(msg->data), 0, SIZE);
    return msg;
}

struct message *file_upload_success_ACK(char *source, char *destination) {
    struct message *msg = (struct message *)malloc(MSGSIZE);
    msg->head.type = 26;
    strcpy(msg->head.source, source);
    strcpy(msg->head.destination, destination);
    msg->head.length = 0;
    msg->head.message_id = 0;
    memset((char *)(msg->data), 0, SIZE);
    return msg;
}

struct message *file_doesnt_exist(char *source, char *destination) {
    struct message *msg = (struct message *)malloc(MSGSIZE);
    msg->head.type = 27;
    strcpy(msg->head.source, source);
    strcpy(msg->head.destination, destination);
    msg->head.length = 0;
    msg->head.message_id = 0;
    memset((char *)(msg->data), 0, SIZE);
    return msg;
}

struct message *file_transfer_complete_ack(char *source, char *destination) {
    struct message *msg = (struct message *)malloc(MSGSIZE);
    msg->head.type = 30;
    strcpy(msg->head.source, source);
    strcpy(msg->head.destination, destination);
    msg->head.length = 0;
    msg->head.message_id = 0;
    memset((char *)(msg->data), 0, SIZE);
    return msg;
}

/****************************************************
*
* Message Handler.
*
*****************************************************/

FILE *fp;
struct stat st;

int file_upload_handler(struct message *buffer, int sockfd) {
    char* filename = buffer->data;
    const char split[2] = "|";
    char* token;
    int sent;
    int file_size;
    int sizetosend;
    int nbytes;
    int z;


    token = strtok(filename, split);

    fstat(fileno(fp), &st);
    file_size = st.st_size;
    printf("what is file size: %d\n", file_size);

    fp = fopen(token, "r");
    sent = 0;

    while (sent != file_size) {
        if (file_size - sent >= 400) {
            sizetosend = 400;
        }
        else {
            sizetosend = file_size - sent;
        }
        struct message *msg = file_transfer(buffer->head.destination, buffer->head.source, sizetosend);
        fseek(fp, sent, SEEK_SET); //move file pointer to correct position to fetch the data.
        z = fread(msg->data, 1, sizetosend, fp); //read from file pointer position, read 512 bytes.
        nbytes = send(sockfd, (char*)msg, 50 + sizetosend, 0);
        sent += sizetosend;
    }

    struct message *msg1 = file_upload_success_ACK(buffer->head.destination, buffer->head.source);
    nbytes = send(sockfd, (char*)msg1, 50, 0);
    return 1;
}

int file_transfer_request_handler(struct message *buffer, int sockfd) {
    char* filename = buffer->data;
    const char split[2] = "|";
    char* token;
    int sent;
    int file_size;
    int sizetosend;
    int nbytes;
    int z;

    token = strtok(filename, split);

    fp = fopen(token, "r");
    sent = 0;

    if (fp == NULL) {
        struct message *msg = file_doesnt_exist(buffer->head.destination, buffer->head.source);
        nbytes = send(sockfd, (char*)msg, 50, 0);
        return 1;
    }
    else { //if not found, send file doesn't exist error.
        fstat(fileno(fp), &st);
        file_size = st.st_size;
        printf("what is file size: %d\n", file_size);
        //send file content to client

        while (sent != file_size) {
            if (file_size - sent >= 400) {
                sizetosend = 400;
            }
            else {
                sizetosend = file_size - sent;
            }
            struct message *msg = file_transfer(buffer->head.destination, buffer->head.source, sizetosend);
            fseek(fp, sent, SEEK_SET); //move file pointer to correct position to fetch the data.
            z = fread(msg->data, 1, sizetosend, fp); //read from file pointer position, read 512 bytes.
            nbytes = send(sockfd, (char*)msg, 50 + sizetosend, 0);
            sent += sizetosend;
        }

        struct message *msg1 = file_transfer_complete_ack(buffer->head.destination, buffer->head.source);
        nbytes = send(sockfd, (char*)msg1, 50, 0);
        return 1;
    }

    return 0;
}

int file_list_handler(struct message *buffer) {

    const char split[2] = "|";
    char* token;

    if (strlen(buffer->data) == 0)
    {
        return 0;
    }

    token = strtok(buffer->data, split);
    while (token != NULL) {
        fprintf(stderr, "file_name is: %s, MD5 = ", token);
        token = strtok(NULL, split);
        fprintf(stderr, "%s\n", token);
        token = strtok(NULL, split);
    }
    return 1;
}

int transfer_ack_handler(struct message *buffer, int sockfd) {

    // if (strlen(buffer->data != 0))
    // {
    //     char portno[5] = buffer->data;
    //do multithread handling.
    return 1;
    //}
}


/****************************************************
*
* error function.
*
*****************************************************/
/*
 * error - wrapper for perror
 */
void error(char *msg) {
    perror(msg);
    exit(0);
}

/****************************************************
*
* Overall Function List.
*
*****************************************************/

unsigned char * MD5_hash_generator(char *file_name) {

    char *filename = file_name;
    int i;
    FILE *inFile = fopen(filename, "rb");
    MD5_CTX mdContext;
    int bytes;
    unsigned char data[1024];
    bzero(md, MD5_DIGEST_LENGTH);
    if (inFile == NULL) {
        printf("%s can't be opened.\n", filename);
        return 0;
    }

    MD5_Init(&mdContext); //initializes a MD5_CTX structures.
    while ((bytes = fread(data, 1, 1024, inFile)) != 0) //called repeatedly via while loop to hash the entire file.
        MD5_Update(&mdContext, data, bytes);
    // places the message digest in md(unsigned char), which must have space for MD5_DIGEST_LENGTH == 16 bytes of output,
    // and erases the MD5_CTX.
    MD5_Final(md, &mdContext);
    for (i = 0; i < MD5_DIGEST_LENGTH; i++) printf("%02x", md[i]); //print out the MD5 hash.
    printf(" %s\n", filename);
    fclose(inFile);
    return md;
}

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;

    if (realsize > WEB_SIZE)
    {
        printf("Webpage is too large!\n");
        return 0;
    }

    memcpy(userp, contents, realsize);

    return realsize;
}

int Intital_Profile(char * webdata)
{
    char buff[2][50] = { 0 };
    FILE * fp = fopen("profile.txt", "r");//opeb states_b
    if (fp == NULL) {
        puts("open erro");
        return -1;
    }
    for (int i = 0; i < 50; i++)
    {
        fgets(buff[i], 50, fp);
    }
    fclose(fp);

    bzero(my_prof.name, 20);
    memcpy(my_prof.name, buff[0], strlen(buff[0]) - 2);

    bzero(my_prof.introduction, 50);
    memcpy(my_prof.introduction, buff[1], strlen(buff[1]) - 2);

    char input[20] = { 0 };

    bzero(my_prof.ip, 20);
    SearchProcess(webdata, "ip", my_prof.ip);

    bzero(my_prof.continent, 20);
    SearchProcess(webdata, "continent_code", my_prof.continent);

    bzero(my_prof.country, 20);
    SearchProcess(webdata, "country_code", my_prof.country);

    bzero(my_prof.region, 20);
    SearchProcess(webdata, "region_code", my_prof.region);

    bzero(my_prof.city, 20);
    SearchProcess(webdata, "city", my_prof.city);

    bzero(my_prof.language, 20);
    SearchProcess(webdata, "native", my_prof.language);

    my_prof.la = 0;
    SearchProcess_f(webdata, "latitude", &my_prof.la);

    my_prof.lo = 0;
    SearchProcess_f(webdata, "longitude", &my_prof.lo);

    printf("Name: %s\n", my_prof.name);
    printf("IP: %s\n", my_prof.ip);
    printf("Curent location: %s, %s, %s, %s\n", my_prof.city, my_prof.region, my_prof.country, my_prof.continent);
    printf("Language: %s\n", my_prof.language);
    printf("Self-intoduction: \n%s\n", my_prof.introduction);

    testpro_prof();
    return 0;
    /*
    printf("Anything wrong?(Y/N)\n");
    gets(input);

    if (input[0] == 'N')
    {
        testpro_prof();
        return 0;
    }
    else
    {
        bzero(input, 20);
        printf("City_name?(press ! to go next)\n");
        gets(input);
        if (input[0] != '!')
        {
            bzero(my_prof.city, 20);
            memcpy(my_prof.city, input, 20);
        }

        bzero(input, 20);
        printf("Language?(press ! to go next)\n");
        gets(input);
        if (input[0] != '!')
        {
            bzero(my_prof.language, 20);
            memcpy(my_prof.language, input, 20);
        }

        testpro_prof();
        return 1;
    }*/

}

int SearchProcess(char * data, char * type, char * out)
{
    char * type_p = 0;
    char * type_dp = 0;
    char * type_ep = 0;
    char * delim = ":\"";
    char * delim2 = "\"";
    int len = 0;

    type_p = strstr(data, type);
    if (type_p == NULL)
    {
        printf("Can't find type");
        return -1;
    }

    type_dp = strstr(type_p, delim);
    type_dp = type_dp + 2;

    type_ep = strstr(type_dp, delim2);
    len = type_ep - type_dp;

    memcpy(out, type_dp, len);

    return len;
}

int testpro_prof()
{
    struct Header temp;

    bzero(&temp, sizeof(struct Header));

    bzero(buf, BUFF_SIZE);

    temp.type = 13;
    temp.length = sizeof(struct profile_format);
    temp.id = 0;
    sprintf(temp.src, "%s", name);
    sprintf(temp.dest, "Server");


    memcpy(buf, &temp, HEAD_SIZE);
    memcpy(buf + HEAD_SIZE, &my_prof, temp.length);

    ReadySend(buf);
    send(sockfd, buf, temp.length + HDRSIZE, 0);

    return temp.length + HDRSIZE;
}

int testpro_Sp(char * name)
{
    struct Header temp;

    bzero(&temp, sizeof(struct Header));

    bzero(buf, BUFF_SIZE);

    temp.type = 14;
    temp.length = 20;
    temp.id = 0;
    sprintf(temp.src, "%s", name);
    sprintf(temp.dest, "Server");


    memcpy(buf, &temp, HEAD_SIZE);
    memcpy(buf + HEAD_SIZE, name, 20);

    ReadySend(buf);
    send(sockfd, buf, 70, 0);

    return 70;
}

void Search_byW(char *Skey) {

    struct Header temp;
    bzero(&temp, sizeof(struct Header));
    bzero(buf, BUFF_SIZE);

    temp.type = 46;
    temp.length = 20;
    temp.id = 0;
    sprintf(temp.src, "%s", name);
    sprintf(temp.dest, "Server");

    memcpy(buf, &temp, HEAD_SIZE);
    memcpy(buf + HEAD_SIZE, Skey, 20);

    ReadySend(buf);
    send(sockfd, buf, 70, 0);
}

void Search_byT(char *Skey) {

    struct Header temp;
    bzero(&temp, sizeof(struct Header));
    bzero(buf, BUFF_SIZE);

    temp.type = 47;
    temp.length = 20;
    temp.id = 0;
    sprintf(temp.src, "%s", name);
    sprintf(temp.dest, "Server");

    memcpy(buf, &temp, HEAD_SIZE);
    memcpy(buf + HEAD_SIZE, Skey, 20);

    ReadySend(buf);
    send(sockfd, buf, 70, 0);

}


void Search_byU(char *Skey) {

    struct Header temp;
    bzero(&temp, sizeof(struct Header));
    bzero(buf, BUFF_SIZE);

    temp.type = 48;
    temp.length = 20;
    temp.id = 0;
    sprintf(temp.src, "%s", name);
    sprintf(temp.dest, "Server");

    memcpy(buf, &temp, HEAD_SIZE);
    memcpy(buf + HEAD_SIZE, Skey, 20);

    ReadySend(buf);
    send(sockfd, buf, 70, 0);

}


int SearchProcess_f(char * data, char * type, double *out)
{
    char * type_p = 0;
    char * type_dp = 0;
    char * type_ep = 0;
    char * delim = "\":";
    char * delim2 = ",";
    int len = 0;
    char temp[20] = { 0 };

    type_p = strstr(data, type);
    if (type_p == NULL)
    {
        printf("Can't find type");
        return -1;
    }

    type_dp = strstr(type_p, delim);
    type_dp = type_dp + 2;

    type_ep = strstr(type_dp, delim2);
    len = type_ep - type_dp;

    memcpy(temp, type_dp, len);
    *out = atof(temp);

    return len;
}