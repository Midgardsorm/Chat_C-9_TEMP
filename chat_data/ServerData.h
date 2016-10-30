#pragma once

#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS 1

#include <iostream>
#include <stdio.h>
#include <winsock2.h>
#include "stdlib.h"
#include <Windows.h>
#include "functions.h"	


class Data {
private:
	WSADATA wsa;
	SOCKET main_socket, new_socket, client_socket[30], tmp_socket;
	sockaddr_in server, address;
	int max_clients, activity, addrlen, read_value, buffsize, port, i;
	char *message, *buffer;
	fd_set fdset_socket; //socket table
public:
	Data();


	~Data();

	int winsockInit();
	int socketInit();
	void adressInit();
	int bindInit();
	void listenInit();
	void setFD();
	int isSmthActive();
	int sendMsg();
	void isNewConnection();
	void addSocket();
	void msgExchange();
	SOCKET getSocket();
	void broadcastMsg();
};
