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
	// ��λ��
	if (count == 2)
		n = number[0] * 10 + number[1];
	// һλ��
	else
		n = number[0];
	return n;
}
int main(int argc, char** argv)
{
	// �����׽���
	// ȷ��ǰ�������ͣ���ϵͳ�Զ�����Э������
	int sock = socket(AF_INET, SOCK_STREAM, 0);

	// bind
	// ���׽��ֺ��ض���IP��ַ�Ͷ˿ڰ�����
	// ������һ�� sockaddr_in �ṹ�壬����ֵ
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
	// ʹ�׽��ֽ��뱻������״̬
	// ��ϵͳ�����������
	if (listen(sock, SOMAXCONN) == -1)
	{
		printf("listen() fail!");
		exit(0);
	}
	// ����ѭ���Բ��Ͻ�������
	while (1)
	{
		// accpet
		// ���׽��ִ��ڼ���״̬ʱ�����ܿͻ�������
		// accept ������Ҫ�Ĳ���
		struct sockaddr_in client_data;
		socklen_t client_data_size = sizeof(client_data);
		// accept �᷵��һ���׽������ںͿͻ���ͨ��
		int sock_s = accept(sock, (struct sockaddr*)&client_data, &client_data_size);
		// sock_s > 0 ʱ�����ӽ�����ͻ��˽��н����������̽�����һ������
		if (sock_s > 0)
		{
			// �����ӽ���
			pid_t pid = fork();
			// �ӽ��̴�������
			if (pid == 0)
			{
				printf("Player %d join the game!\n", getpid());
				// ���������
				srand((unsigned)time(NULL));
				int num = rand() % 100;
				printf("Player %d get a num :%d\n", getpid(), num);

				// ��ͻ��˷��������Ϣ
				char player[20];
				// ���� ID
				// ����ǿ������ת�� int -> str
				sprintf(player, "%d", getpid());
				write(sock_s, player, sizeof(player));

				// ��ȡ�ͻ��˷�����������
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
					// �Ƚϴ�С
					if (n == num)
					{
						strcpy(buffer, "TQL,You guessed it!!!");
						write(sock_s, buffer, sizeof(buffer));
						printf("Player %d disconnects\n", getpid());
						// �¶����˳�
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
				// �����̵ȴ��ӽ��̽����󣬽�������
				//if (waitpid(pid, NULL, 0) > 0)
				//{
					close(sock_s);
				//}

			}
		}
		// ����������ֱ���˳�ѭ��
		else
		{
			printf("accept fail!\n");
			break;
		}

	}
	// �ر�����׽���֮�󣬻��������
	// ����ִ�й����г����˴��󣬷����һ�㲻�������رգ���������ָ��һ�㲻��ִ��
	close(sock);




	return 0;
}