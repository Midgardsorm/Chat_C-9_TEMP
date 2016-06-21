#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS 1

#include <iostream>
#include <process.h>
#include <winsock2.h>


//klasa zawierajaca dane przesylane do watkow przez wskaznik
class Data {
public:
	SOCKET mainSocket;
	char buffer[1000];
};

//klasa dla funkcji inicjalizuj¹cych i obslugujacych gniazda
class Server {
private:
	const char *m_ip;
	int m_port;
	WSADATA m_wsaData;
	Data *mainData;
	sockaddr_in service;
public:
	Server(Data *data) : m_ip("127.0.0.1"), m_port(27015), mainData(data)
	{

	}

	~Server()
	{
		std::cout << "Zamkniecie gniazda.";
	}
	
	int winsockInit();
	int socketInit();
	void adressInit();
	int bindInit();
	void listenInit();
	void clientInit();
};

int Server::winsockInit()
{
	int result = WSAStartup(MAKEWORD(2, 2), &m_wsaData);
	if (result != NO_ERROR)
	{
		std::cout << "Nie mozna zainicjowac gniazda.";
		return 1;
	}
	return 0;
}

int Server::socketInit()
{
	mainData->mainSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (mainData->mainSocket == INVALID_SOCKET)
	{
		std::cout << "Blad przy tworzeniu gniazda: %ld\n" << WSAGetLastError();
		WSACleanup();
		return 1;
	}
	return 0;
}

void Server::adressInit()
{
	memset(&service, 0, sizeof(service));
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = inet_addr(m_ip);
	service.sin_port = htons(m_port);
}

int Server::bindInit()
{
	int binding = bind(mainData->mainSocket, (SOCKADDR *)& service, sizeof(service));
	if (binding == SOCKET_ERROR)
	{
		std::cout << "Przypisanie adresu bind() nieudane.\n";
		closesocket(mainData->mainSocket);
		return 1;
	}
	return 0;
}


void Server::listenInit()
{
	int listening = listen(mainData->mainSocket, 1);
	if (listening == SOCKET_ERROR)
	{
		std::cout << "Blad nasluchu na gniezdzie.\n";
	}
}


void Server::clientInit()
{
	SOCKET acceptSocket = SOCKET_ERROR;
	std::cout << "Oczekiwanie na klienta\n";

	while (acceptSocket == SOCKET_ERROR)
	{
		acceptSocket = accept(mainData->mainSocket, NULL, NULL);
	}

	std::cout << "Klient podlaczony.\n";
	mainData->mainSocket = acceptSocket;
}

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
		std::cout << "Klient: " << someData->buffer << "\n";
	} while (1);
	
	_endthread();
}

void sendMsg(Data *mainData)
{
	do {
		std::cin.getline(mainData->buffer, 1000);
		send(mainData->mainSocket, mainData->buffer, 1000, 0);
	} while (*(mainData->buffer) != '#');

	std::cout << "Koniec transmisji.\n";
}

int main()
{
	Data* mainData = new Data;
	
	Server server(mainData);

	server.winsockInit();
	server.socketInit();
	server.adressInit();
	server.bindInit();
	server.listenInit();
	server.clientInit();

	
	//watki:
	//nasluchuje wiadomosci od podlaczonych klientow
	_beginthread(listenForMsg, 0, mainData);

	//wysylanie wiadomosci i obs³uga chatu
	sendMsg(mainData);

	//czyszczenie pamieci
	delete mainData;
	mainData = 0;

	return 0;
}