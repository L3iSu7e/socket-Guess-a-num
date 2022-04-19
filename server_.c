#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/wait.h>
#include <time.h>
int strTonum(char buff[])
{
	int len = strlen(buff);
	int i,count = 0,n;
	int number[10];
	for (i = 0; i < len; i++)
	{
		if (buff[i] >= 48 && buff[i] <= 57)
			number[count++] = (int)(buff[i] - 48);
	}
	// 两位数
	if (count == 2)
		n = number[0] * 10 + number[1];
	// 一位数
	else
		n = number[0];
	return n;
}
int main(int argc, char** argv)
{
	// 创建套接字
	// 确定前两种类型，让系统自动推演协议类型
	int sock = socket(AF_INET, SOCK_STREAM, 0);

	// bind
	// 将套接字和特定的IP地址和端口绑定起来
	// 先声明一个 sockaddr_in 结构体，并赋值
	struct sockaddr_in addr;
	//memset()
	addr.sin_family = AF_INET;
	addr.sin_port = htons(1234);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) == -1)
	{
		printf("bind() fail!\n");
		exit(0);
	}
	// listen
	// 使套接字进入被动监听状态
	// 由系统决定请求队列
	if (listen(sock, SOMAXCONN) == -1)
	{
		printf("listen() fail!");
		exit(0);
	}
	// 无限循环以不断接受请求
	while (1)
	{
		// accpet
		// 当套接字处于监听状态时，接受客户端请求
		// accept 函数需要的参数
		struct sockaddr_in client_data;
		socklen_t client_data_size = sizeof(client_data);
		// accept 会返回一个套接字用于和客户端通信
		int sock_s = accept(sock, (struct sockaddr*)&client_data, &client_data_size);
		// sock_s > 0 时，用子进程与客户端进行交互，父进程接受下一个请求
		if (sock_s > 0)
		{
			// 创建子进程
			pid_t pid = fork();
			// 子进程处理请求
			if (pid == 0)
			{
				printf("Player %d join the game!\n", getpid());
				// 生成随机数
				srand((unsigned)time(NULL));
				int num = rand() % 100;
				printf("Player %d get a num :%d\n", getpid(), num);

				// 向客户端发送相关信息
				char player[20];
				// 发送 ID
				// 进行强制类型转换 int -> str
				sprintf(player, "%d", getpid());
				write(sock_s, player, sizeof(player));

				// 读取客户端发过来的数据
				while (1)
				{
					char buffer[100];
					read(sock_s, buffer, sizeof(buffer));
					if (!strcmp(buffer, "quit"))
					{
						printf("Player %d disconnects\n", getpid());
						close(sock_s);
						exit(0);
					}
					printf("Player %d guessed the number: %s(%d)\n", getpid(),buffer,num);
					// str->num
					int n = strTonum(buffer);
					printf("n = %d\n", n);
					// 比较大小
					if (n == num)
					{
						strcpy(buffer, "TQL,You guessed it!!!");
						write(sock_s, buffer, sizeof(buffer));
						printf("Player %d disconnects\n", getpid());
						// 猜对了退出
						close(sock_s);
						exit(0);
					}
					else if (n > num)
					{
						strcpy(buffer, "You guessed the number is big~");
						write(sock_s, buffer, sizeof(buffer));
					}
					else
					{
						strcpy(buffer, "You guessed the number is small~");
						write(sock_s, buffer, sizeof(buffer));
					}

				}

			}
			else
			{
				// 父进程等待子进程结束后，结束交互
				//if (waitpid(pid, NULL, 0) > 0)
				//{
					close(sock_s);
				//}

			}
		}
		// 出错的情况，直接退出循环
		else
		{
			printf("accept fail!\n");
			break;
		}

	}
	// 关闭这个套接字之后，会结束监听
	// 除非执行过程中出现了错误，服务端一般不会主动关闭，所以这条指令一般不会执行
	close(sock);




	return 0;
}