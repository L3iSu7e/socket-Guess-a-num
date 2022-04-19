#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
int main(int argc, char** argv)
{
    // 创建套接字
    // 让系统根据前两个参数推演协议类型
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    // connect 将套接字与特定的IP地址和端口绑定起来
    // 声明一个 struct sockaddr_in 类型
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(1234);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    // connect 返回0 表示连接成功， 返回 -1 则是连接失败
    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) == -1)
    {
        printf("connect fail!\n");
        close(sock);
    }
    // 连接成功，开始交互
    else
    {
        // read/write 
        char buffer[100];
        // 读取 ID
        read(sock, buffer, sizeof(buffer));
        printf("Welcome!Player %s\n", buffer);
        printf("If you want to leave,please input quit\n");
        // 猜数环节
        while (1)
        {
            printf("Please enter a number between 0 and 100(not including 100): ");
            scanf("%s", buffer);
            write(sock, buffer, sizeof(buffer));
            // 用户退出，结束交互
            if (!strcmp(buffer, "quit"))
            {
                close(sock);
                sleep(3);
                exit(0);
            }
            //读取服务端返回值
            read(sock, buffer, sizeof(buffer));
            printf("%s\n", buffer);
            if (!strcmp(buffer, "TQL,You guessed it!!!"))
            {
                printf("Game over...");
                close(sock);
                sleep(3);
                exit(0);
            }
        }
        

    }





    return 0;
}