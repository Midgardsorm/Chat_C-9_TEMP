#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS 1

#include <iostream>
#include <process.h>
#include <winsock2.h>
#include <Windows.h>


//klasa zawierajπca dane przesy≥ane do watkow przez wskaünik
class Data {
public:
	SOCKET mainSocket;
	char buffer[1000];
	bool isEnd;

	Data() : isEnd(false)
	{
	}
};

//klasa dla funkcji inicjalizujπcych i obslugujacych gniazda
class Client {
private:
	const char *m_ip;
	int m_port;
	WSADATA m_wsaData;
	Data *mainData;
	sockaddr_in service;
public:
	Client(Data *data) : m_ip("127.0.0.1"), m_port(27015), mainData(data)
	{

	}

	~Client()
	{
		std::cout << "Zamkniecie gniazda.";
	}

	int winsockInit();
	int socketInit();
	void adressInit();
	int serverConnect();
};

int Client::winsockInit()
{
	int result = WSAStartup(MAKEWORD(2, 2), &m_wsaData);
	if (result != NO_ERROR)
	{
		std::cout << "Nie moøna zainicjowac gniazda.";
		return 1;
	}
	return 0;
}

int Client::socketInit()
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

void Client::adressInit()
{
	memset(&service, 0, sizeof(service));
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = inet_addr(m_ip);
	service.sin_port = htons(m_port);
}

int Client::serverConnect()
{
	int connected = connect(mainData->mainSocket, (SOCKADDR *)& service, sizeof(service));
	if (connected == SOCKET_ERROR)
	{
		std::cout << "Nieudane podlaczenie.\n";
		WSACleanup();
		return 1;
	}
std::cout << "Podlaczenie do serwera udane.\nNacisnij Esc aby zakonczyc program.\n";
	return 0;
}

void listenForMsg(void * mainData)
{
	Data *someData = reinterpret_cast<Data*>(mainData);
	do {
		int bytesRecv = SOCKET_ERROR;
		//petla nasluchujaca wiadomoúci
		while (bytesRecv == SOCKET_ERROR)
		{
			bytesRecv = recv(someData->mainSocket, someData->buffer, 1000, 0);
		}
		std::cout << "Serwer: " << someData->buffer << "\n";
	} while (someData->isEnd == false);

	_endthread();
}

void waitForEsc(void * mainData)
{
	Data *someData = reinterpret_cast<Data*>(mainData);
	while (true)
	{
		if (GetAsyncKeyState(VK_ESCAPE))
		{
			someData->isEnd = true;
			std::cout << "Nacisnij Enter, aby wyjsc z programu.\n";
			_endthread();
		}
	}
}

void sendMsg(Data *mainData)
{
	do {
		std::cin.getline(mainData->buffer, 1000);
		send(mainData->mainSocket, mainData->buffer, 1000, 0);
	} while (mainData->isEnd == false);

	std::cout << "Koniec transmisji.\n";
}



int main()
{
	Data *mainData = new Data;
	
	Client client(mainData);

	client.winsockInit();
	client.socketInit();
	client.adressInit();
	client.serverConnect();

	
	//watki:
	//nasluchuje wiadomosci od podlaczonych klientow
	_beginthread(listenForMsg, 0, mainData);
	
	_beginthread(waitForEsc, 0, mainData);

	//wysylanie wiadomosci i obs≥uga chatu
	sendMsg(mainData);

	//czyszczenie pamieci
	delete mainData;
	mainData = 0;

	
	return 0;
}
