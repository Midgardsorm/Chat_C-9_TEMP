#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS 1

#include <iostream>
#include <process.h>
#include <winsock2.h>

SOCKET mainSocket;

class Data {
public:
	SOCKET mainSocket;
	char buffer[1000];
};


void listenForMsg(void * mainData)
{
	do {
		int bytesRecv = SOCKET_ERROR;
		Data *someData = reinterpret_cast<Data*>(mainData);
		//petla nasluchujaca wiadomo�ci
		while (bytesRecv == SOCKET_ERROR)
		{
			bytesRecv = recv(someData->mainSocket, someData->buffer, 1000, 0);
		}
		std::cout << "Server: " << someData->buffer << "\n";
	} while (1);

	_endthread();
}

int main()
{
	Data* mainData = new Data;
	
	//inicjalizacja winsocka
	WSADATA wsaData;

	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != NO_ERROR)
	{
		std::cout << "Nie mo�na zainicjowa� gniazda";
	}

	//inicjalizacja gniazda
	mainData->mainSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (mainData->mainSocket == INVALID_SOCKET)
	{
		std::cout << "Error creating socket: %ld\n" << WSAGetLastError();
		WSACleanup();
		return 1;
	}
	
	//podanie ip i portu
	sockaddr_in service;
	memset(&service, 0, sizeof(service));
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = inet_addr("127.0.0.1");
	service.sin_port = htons(27015);

	//podlaczanie do serwera
	int connected = connect(mainData->mainSocket, (SOCKADDR *)& service, sizeof(service));
	if (connected == SOCKET_ERROR)
	{
		printf("Failed to connect.\n");
		WSACleanup();
		return 1;
	}

		
	//watki:
	//nasluchuje wiadomosci od podlaczonych klientow
	_beginthread(listenForMsg, 0, mainData);
	
	//wysylanie wiadomosci i obs�uga chatu
	do {
		std::cin.getline(mainData->buffer, 1000);
		send(mainData->mainSocket, mainData->buffer, 1000, 0);
	} while (*(mainData->buffer) != '#');

	std::cout << "Ending trasmission ... \n";

	//czyszczenie pamieci
	delete mainData;
	mainData = 0;

	return 0;
}
