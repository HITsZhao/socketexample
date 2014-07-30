#include <netinet/in.h>     // for sockaddr_in
#include <sys/types.h>      // for socket
#include <sys/socket.h>     // for socket
#include <arpa/inet.h>
#include <stdio.h>          
#include <stdlib.h>           
#include <string.h>
#include <errno.h>
#include <unistd.h>
#define SERVER_PORT     8000
#define BUFFER_SIZE     1024
#define FILE_NAME_MAX_SIZE  512

int main(int argc, char **argv)
{
    if(2 != argc)
    {
        printf("Usage: %s ServerIPAddress\n",argv[0]);
        exit(1);
    }
    //设置一个socket地址结构client_addr,代表客户端internet地址，端口
    struct sockaddr_in client_addr;
    memset(&client_addr,0,sizeof(client_addr)); //把内存区内容置0
    //初始化sockaddr_in结构体
    client_addr.sin_family = AF_INET;               //internet协议族
    client_addr.sin_addr.s_addr = htons(INADDR_ANY);//INADDR_ANY 自动获取IP地址
    client_addr.sin_port = htons(0);      //0表示让系统自动分配一个端口

    //创建用于internet的流协议(TCP)socket,用client_socket代表客户端socket
    int client_socket = socket(PF_INET,SOCK_STREAM,0); 
    if(client_socket < 0)
    {
        printf("Creat client socket failed !\n");
        exit(1);
    }
    if(0 != bind(client_socket,(struct sockaddr*)&client_addr,sizeof(client_addr)))
    {
        printf("Bind client socket failed !\n");
        exit(1);
    }
    //创建一个sockaddr_in结构体server_addr,代表服务器internet地址，端口
    struct sockaddr_in server_addr;
    memset(&server_addr,0,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    /*if(inet_aton(argv[1],&server_addr.sin_addr) == 0) //从程序参数获得服务器IP地址
    {
        printf("Get server IP Error!\n");
        exit(1);
    }*/
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(SERVER_PORT);
    //socklen_t server_addr_length = sizeof(server_addr);

    //向服务端发起连接，连接成功后client_socket代表了客户端和服务器之间的一个连接
    if(connect(client_socket,(struct sockaddr*)&server_addr,sizeof(server_addr)) < 0)
    {
        printf("connet to %s failed !,error code %s\n",argv[1],strerror(errno));
        exit(1);
    }    
    
    //向服务器发送请求
    char file_name[FILE_NAME_MAX_SIZE + 1];
    memset(file_name,0,FILE_NAME_MAX_SIZE + 1);
    printf("Please input the file name required in server:\t");
    scanf("%s",file_name);
    char buffer[BUFFER_SIZE];
    memset(buffer,0,BUFFER_SIZE);
    strncpy(buffer,file_name,strlen(file_name)>BUFFER_SIZE? BUFFER_SIZE:strlen(file_name));
    //向服务器发送buffer中的数据
    send(client_socket,buffer,BUFFER_SIZE,0);
    
    //接收并保存数据
    //char file_rev_name[FILE_NAME_MAX_SIZE + 1];
    //memset(file_rev_name,0,FILE_NAME_MAX_SIZE + 1);
    const char *file_rev_name = "revf";
    FILE *pFile = fopen(file_rev_name,"w");
    if(NULL == pFile)
    {
        printf("File:\t%s can not open",file_rev_name);
        exit(1);
    }

    memset(buffer,0,BUFFER_SIZE);
    int length = 0;
    while((length = recv(client_socket,buffer,BUFFER_SIZE,0)))
    {
        if(length < 0)
        {
            printf("Receive data from server failed !\n");
            exit(1);
        }
        int write_length = fwrite(buffer,sizeof(char),length,pFile);
        if(write_length < length)
        {
            printf("File:\t%s write failed!\n",file_name);
            break;
        }
        memset(buffer,0,sizeof(BUFFER_SIZE));
    }

    printf("socket: receive file : %s from server [%s] finished !\n",file_name,argv[1]);
    fclose(pFile);
    //关闭socket
    close(client_socket);
    return 0;
}
