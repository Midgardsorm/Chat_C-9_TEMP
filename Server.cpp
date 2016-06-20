#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS 1

#include <iostream>
#include <process.h>
#include <winsock2.h>



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
		//petla nasluchujaca wiadomoœci
		while (bytesRecv == SOCKET_ERROR)
		{
			bytesRecv = recv(someData->mainSocket, someData->buffer, 1000, 0);
		}
		std::cout << "Client: " << someData->buffer << "\n";
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
		std::cout << "Nie mo¿na zainicjowaæ gniazda";
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

	//bindowanie adresu i portu do gniazda
	int binding = bind(mainData->mainSocket, (SOCKADDR *)& service, sizeof(service));
	if (binding == SOCKET_ERROR)
	{
		std::cout << "bind() failed.\n";
		closesocket(mainData->mainSocket);
		return 1;
	}

	//wlaczenie funkcji nasluchowej
	int listening = listen(mainData->mainSocket, 1);
	if (listening == SOCKET_ERROR)
	{
		std::cout << "Error listening on socket.\n";
	}
	
	//podlaczenie klienta i nasluchiwanie
	SOCKET acceptSocket = SOCKET_ERROR;
	std::cout << "Waiting for a client to connect...\n";

	while (acceptSocket == SOCKET_ERROR)
	{
		acceptSocket = accept(mainData->mainSocket, NULL, NULL);
	}

	std::cout << "Client connected.\n";
	mainData->mainSocket = acceptSocket;

	
	//watki:
	//nasluchuje wiadomosci od podlaczonych klientow
	_beginthread(listenForMsg, 0, mainData);

	//wysylanie wiadomosci i obs³uga chatu
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