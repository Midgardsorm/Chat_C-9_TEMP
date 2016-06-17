#define WIN32_LEAN_AND_MEAN						//przy u¿yciu windows.h wyrzucamy windows socketes, by unikn¹æ k³ótni z winsock2
#define _WINSOCK_DEPRECATED_NO_WARNINGS 1

#include <iostream>
#include <process.h>							//dla w¹tków
#include <winsock2.h>							//dla socketów + ws2_32.lib dodany do projektu
#include <conio.h>								//dla getch()

SOCKET mainSocket;

class sendFlag									//ustawia flagê podaj¹c¹, czy wywo³aæ w¹tek wysy³aj¹cy
{												//utworzony dynamicznie bêdzie wspóldzielony miêdzy g³ównym w¹tkiem wysy³aj¹cym a nas³uchuj¹cym
private:
	bool toSend;								//jesli przyjdzie jakaœ wiadomoœæ, w¹tek nas³uchuj¹cy ustawi t¹ zmienn¹ na wartoœæ true
public:											//a po wys³aniu do wszystkich wiadomoœci ustawi ponownie na false
	sendFlag()
		: toSend(false)
	{

	}
};

void listenForMsg(void * buffer)
{
	do {
		int bytesRecv = SOCKET_ERROR;
		while (bytesRecv == SOCKET_ERROR)		//pêtla nas³uchuj¹ca wiadomoœci
		{
			bytesRecv = recv(mainSocket, reinterpret_cast<char*>(buffer), 1000, 0);
		}
		std::cout << "Server: " << reinterpret_cast<char*>(buffer) << "\n";
		Sleep(1000);
	} while (1);

	_endthread();
}

void sendMsg(void * buffer)
{


	do {
		std::cin.getline(reinterpret_cast<char*>(buffer), 1000);
		send(mainSocket, reinterpret_cast<char*>(buffer), 1000, 0);
	} while (*reinterpret_cast<char*>(buffer) != '#');

	std::cout << "Ending trasmission ... \n";


	_endthread();
}

int main()
{

	int buffsize = 1000;
	char* buffer = new char[buffsize];											//bufor przetrzymuj¹cy wiadomoœæ

	
	//inicjalizacja winsocka
	WSADATA wsaData;

	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != NO_ERROR)
	{
		std::cout << "Nie mo¿na zainicjowaæ gniazda";
	}

	//inicjalizacja gniazda
	mainSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (mainSocket == INVALID_SOCKET)
	{
		printf("Error creating socket: %ld\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}
	
	//podanie ip i portu
	sockaddr_in service;
	memset(&service, 0, sizeof(service));
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = inet_addr("127.0.0.1");
	service.sin_port = htons(27015);

	//pod³¹czanie do serwera
	if (connect(mainSocket, (SOCKADDR *)& service, sizeof(service)) == SOCKET_ERROR)
	{
		printf("Failed to connect.\n");
		WSACleanup();
		return 1;
	}

	//
	
	//watki
	
	_beginthread(listenForMsg, 0, buffer);										//nas³uchuje wiadomoœci od pod³¹czonych klientów
	HANDLE watek = (HANDLE)_beginthread(sendMsg, 0, buffer);					//wysy³a wiadomoœæ do klientów

	WaitForSingleObject(watek, INFINITE);										//dopoki klient nie zadecyduje o wylaczeniu, poprzez wyslanie '#'
	
	delete[] buffer;
	buffer = 0;

	_getch();


	return 0;
}
