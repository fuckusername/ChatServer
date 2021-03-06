
#include "pch.h"
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include "winsock2.h"
int numclients = 0;
char buf[256];
char sendbuf[256];
char cmd[16], tmp[64];
char msg[] = "Khong dung cu phap, yeu cau nhap lai ! \n";
char selMess[] = "Moi nhap id theo cu phap: client_id: xxxx\n";
fd_set fdread;
int ret;
SOCKET client;
struct CLIENTCHAT
{
	char id[64];
	SOCKET socket;
};
CLIENTCHAT clients[1024];
CLIENTCHAT clientChat;
int main()
{
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);
	SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(9000);
	bind(listener, (SOCKADDR *)&addr, sizeof(addr));
	listen(listener, 5);


	timeval tv;
	tv.tv_sec = 5;
	tv.tv_usec = 0;
	while (1)
	{
		FD_ZERO(&fdread);
		FD_SET(listener, &fdread);
		for (int i = 0; i < numclients; i++)
		{
			FD_SET(clients[i].socket, &fdread);
		}
		ret = select(0, &fdread, NULL, NULL, &tv);
		if (ret == SOCKET_ERROR)
		{
			printf("ERROR!\n");
			break;
		}
		else if (ret == 0)
		{
		}
		else if (ret > 0)
		{
			if (FD_ISSET(listener, &fdread))
			{
				client = accept(listener, NULL, NULL);
				send(client, selMess, strlen(selMess), 0);
				while (1) {
					ret = recv(client, buf, sizeof(buf), 0);
					if (ret <= 0)
					{
						closesocket(client);
						break;
					}
					else {
						buf[ret] = 0;
						if (buf[strlen(buf) - 1] == '\n')
							buf[strlen(buf) - 1] = 0;

						ret = sscanf(buf, "%s %s %s", cmd, clientChat.id, tmp);
						if (ret == 2)
						{
							if (strcmp(cmd, "client_id:") == 0)
							{
								clientChat.socket = client;
								send(client, "OK\n", 3, 0);
								printf("New client %d", client);
								clients[numclients] = clientChat;
								numclients++;
								break;
							}
							else
								send(client, msg, strlen(msg), 0);
						}
						else
							send(client, msg, strlen(msg), 0);
					}
				}


			}
			for (int i = 0; i < numclients; i++)
			{
				if (FD_ISSET(clients[i].socket, &fdread))
				{
					ret = recv(clients[i].socket, buf, sizeof(buf), 0);
					if (ret <= 0)
					{
						clients[i] = clients[numclients - 1];
						numclients--;
					}
					else {
						buf[ret] = 0;
						if (buf[strlen(buf) - 1] == '\n')
							buf[strlen(buf) - 1] = 0;
						printf("Received %d: %s\n", clients[i].socket, buf);
						sprintf(sendbuf, "%s: %s\n", clients[i].id, buf);
						for (int j = 0; j < numclients; j++)
							if (j != i)
							{
								send(clients[j].socket, sendbuf, strlen(sendbuf), 0);
							}

					}
				}
			}
		}

	}
	closesocket(listener);
	WSACleanup();
	return 0;
}