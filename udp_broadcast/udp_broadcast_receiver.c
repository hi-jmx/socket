#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>

#define BUFF_SIZE 30

int main(int argc ,char *argv[]){
    //声明接收者的
    int receiver_socket;
    //每次读到的字符串长度
    int str_len;
    //用于保存数据的字符缓冲
    char buff[BUFF_SIZE];
    //receiver的地址
    struct sockaddr_in receiver_addr;
    //声明一个用于多播的地址结构体
    struct ip_mreq join_addr;


    int addr_len =sizeof(struct sockaddr_in);
    

    if(argc!= 3){
        printf("Uasge : %s <GroupIP> <PORT> ",argv[0]);
        exit(1);
    }
    //初始化receiver_socket
    receiver_socket = socket(PF_INET,SOCK_DGRAM,0);
    memset(&receiver_addr,0,sizeof(receiver_addr));
    receiver_addr.sin_family = AF_INET;
    receiver_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    receiver_addr.sin_port = htons(atoi(argv[2]));
    //绑定地址
    if(bind(receiver_socket,(struct sockaddr *)&receiver_addr,sizeof(receiver_addr)) == -1){
        error_handling("bind() error");
    }
    //初始化多播地址组
    join_addr.imr_multiaddr.s_addr = inet_addr(argv[1]);//要加入的多播组地址
    join_addr.imr_interface.s_addr = htonl(INADDR_ANY);//加入该组的套接字所属的主机IP地址
    //设置socket中的IP_ADD_MEMBERSHIP选项加入多播组
    setsockopt(receiver_socket,IPPROTO_IP,IP_ADD_MEMBERSHIP,(void *)&join_addr,sizeof(join_addr));

    while(1){
        //接收发送来的消息，因为之前已经将socket注册到组播中
        str_len = recvfrom(receiver_socket,buff,BUFF_SIZE-1,0,(struct sockaddr*)&receiver_addr,&addr_len);
        if(str_len<0){
            break;
        }
        buff[str_len] = 0;
        //使用标准的输入流输出接受到的数据
        printf("%s %d.%d.%d.%d\n", buff, receiver_addr.sin_addr.s_addr&0xFF,  (receiver_addr.sin_addr.s_addr>>8)&0xFF,
					(receiver_addr.sin_addr.s_addr>>16)&0xFF, (receiver_addr.sin_addr.s_addr>>24)&0xFF);
    }
    close(receiver_socket);
return 0;
}

void error_handling(char * message){
    fputs(message,stderr);
    fputc('\n',stderr);
    exit(1);
}