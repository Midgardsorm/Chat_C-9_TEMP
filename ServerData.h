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
	WSADATA m_wsa;
	SOCKET m_main_socket, m_new_socket, m_client_socket[30], m_tmp_socket;
	sockaddr_in m_server, m_address;
	int m_max_clients, m_activity, m_addrlen, m_read_value, m_buffsize, m_port, m_i;
	char *m_message, *m_buffer;
	fd_set m_fdset_socket; //socket table
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
