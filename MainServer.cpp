#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS 1

#include <iostream>
#include <stdio.h>
#include <winsock2.h>
#include "stdlib.h"
#include <process.h>
#include <Windows.h>
#include "functions.h"
#include "Ending.h"
#include "ServerData.h"

int main()
{
	Data chat;

	chat.winsockInit();
	chat.socketInit();
	chat.adressInit();
	chat.bindInit();
	chat.listenInit();

	Ending *EndData = new Ending;
	_beginthread(waitForEsc, 0, EndData);

	while (EndData->isEnd == false)
	{
		chat.setFD();

		chat.isSmthActive();

		//sprawdza czy to nowe polaczenie
		chat.isNewConnection();
			
		//jesli nie, to znaczy ze to przychodzaca wiadomosc od podlaczonego klienta
		chat.msgExchange();
		
	}

	closesocket(chat.getSocket());
	WSACleanup();

	return 0;
}
