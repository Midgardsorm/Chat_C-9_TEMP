#include <iostream>
#include <process.h>							//dla w¹tków
#include <winsock2.h>							//dla socketów + ws2_32.lib dodany do projektu
#include <conio.h>								//dla getch()


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

void isSomeoneThere(void * Args)
{
	do
	{
		std::cout << "kumbaja\n";
		Sleep(1000);
	} while (1);

	_endthread();
}

void listenForMsg(void * Args)
{
	do
	{
		std::cout << "KUKARACZA\n";
		Sleep(10000);
	} while (1);

	_endthread();
}

void sendMsg(void * Args)
{
	do
	{
		std::cout << "kAlABaNgA!\n";
		Sleep(500);
	} while (1);

	_endthread();
}

int main()
{
	


	int buffsize = 1000;
	char* buffer = new char[buffsize];											//bufor przetrzymuj¹cy wiadomoœæ


	//inicjalizacja winsocka
	WSADATA wsaData;															

	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0)
		std::cout << "Nie mo¿na zainicjowaæ gniazda";


	//watki
	_beginthread(isSomeoneThere, 0, NULL);										//w¹tek nas³uchuj¹cy nowych u¿ytkowników
	_beginthread(listenForMsg, 0, NULL);										//nas³uchuje wiadomoœci od pod³¹czonych klientów
	_beginthread(sendMsg, 0, NULL);												//wysy³a wiadomoœæ do klientów



	delete[] buffer;
	buffer = 0;

	_getch();

	return 0;
}