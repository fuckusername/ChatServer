
#include "pch.h"
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include "winsock2.h"
int numclients = 0;
char buf[256];
char msg[] = "Wrong password or id\n";
char firstMsg[] = "Sign in:\n";
fd_set fdread;
int ret;
SOCKET client;
struct CLIENTTelnet
{
	SOCKET socket;
};
CLIENTTelnet clients[1024];
CLIENTTelnet clientTelnet;
int main(int argc, char *argv[])
{
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);
	SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	SOCKADDR_IN addr;
	int port = atoi(argv[1]);
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);
	bind(listener, (SOCKADDR *)&addr, sizeof(addr));
	listen(listener, 5);
	timeval tv;
	tv.tv_sec = 5;
	tv.tv_usec = 0;
	char filebuf[256];
	char cmdbuf[256];
	while (true)
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
				send(client, firstMsg, strlen(firstMsg), 0);
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

						int found = 0;
						FILE *f = fopen("users.txt", "r");
						while (fgets(filebuf, sizeof(filebuf), f))
						{
							if (filebuf[strlen(filebuf) - 1] == '\n')
								filebuf[strlen(filebuf) - 1] = 0;

							if (strcmp(filebuf, buf) == 0)
							{
								found = 1;
								break;
							}
						}
						fclose(f);
						if (found == 1)
						{
							send(client, "OK\n", 3, 0);
							printf("New client %d", client);
							clientTelnet.socket = client;
							clients[numclients] = clientTelnet;
							numclients++;
							break;
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
						sprintf(cmdbuf, "%s > C:\\Users\\QuangTrung\\source\\repos\\Select\\out.txt", buf);
						system(cmdbuf);
						printf("%s", cmdbuf);
						FILE *f = fopen("C:\\Users\\QuangTrung\\source\\repos\\Select\\out.txt", "r");
						while (fgets(filebuf, sizeof(buf), f))
							send(clients[i].socket, filebuf, strlen(filebuf), 0);
						fclose(f);

					}
				}
			}
		}

	}
	closesocket(listener);
	WSACleanup();
	return 0;
}