#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS 1

#include <iostream>
#include <stdio.h>
#include <winsock2.h>
#include "malloc.h"
#include "stdlib.h"
#include <process.h>
#include <Windows.h>

int isSocket(SOCKET socket)
{
	if (socket < 0)
	{
		std::cout << "B³ad funkcji akceptuj¹cej.\n";
		return 1;
	}
	return 0;
}

class Data {
private:
	WSADATA m_wsa;
	SOCKET m_main_socket, m_new_socket, m_client_socket[30], m_tmp_socket;
	sockaddr_in m_server, m_address;
	int m_max_clients, m_activity, m_addrlen, m_read_value, m_buffsize, m_port, m_i;
	char *m_message, *m_buffer;
	fd_set m_fdset_socket; //zestaw socketow
public:
	Data() 
		: m_buffsize(1024), m_max_clients(30), m_addrlen(sizeof(struct sockaddr_in)), m_port(27015),
		 m_message("Chat v.1.0 (Z pomoca asynchronicznego programowania gniazd)  \r\n")
	{
		m_buffer = new char[m_buffsize + 1];

		//ustawienie czystej listy socketów
		for (m_i = 0; m_i < 30; m_i++)
		{
			m_client_socket[m_i] = 0;
		}
	}

	~Data()
	{
	}

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

int Data::winsockInit()
{
	std::cout << m_message << "\nInicjalizacja Winsocka";
	int result = WSAStartup(MAKEWORD(2, 2), &m_wsa);
	if (result != NO_ERROR)
	{
		std::cout << "Nie mozna zainicjowac gniazda.";
		return 1;
	}
	std::cout << "Inicjalizacja udana\n";
	return 0;
}

int Data::socketInit()
{
	m_main_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_main_socket == INVALID_SOCKET)
	{
		std::cout << "Blad przy tworzeniu gniazda: \n" << WSAGetLastError();
		WSACleanup();
		return 1;
	}
	std::cout << "Gniazdo utworzone.\n";
	return 0;
}

void Data::adressInit()
{
	memset(&m_server, 0, sizeof(m_server));
	m_server.sin_family = AF_INET;
	m_server.sin_addr.s_addr = INADDR_ANY;
	m_server.sin_port = htons(m_port);
}

int Data::bindInit()
{
	int binding = bind(m_main_socket, (SOCKADDR *)& m_server, sizeof(m_server));
	if (binding == SOCKET_ERROR)
	{
		std::cout << "Przypisanie adresu bind() nieudane.\n";
		closesocket(m_main_socket);
		return 1;
	}
	std::cout << "Przypisanie adresu bind() udane.\n";
	return 0;
}

void Data::listenInit()
{
	int listening = listen(m_main_socket, 5);
	if (listening == SOCKET_ERROR)
	{
		std::cout << "Blad nasluchu na gniezdzie.\n";
	}
	std::cout << "Oczekiwanie na polaczenia przychodzace \n";
}

//dodawanie socketow do zestawu m_fdset_socket (typ fd_set)
//fd_set uzywane bedzie przez funkcje select()
void Data::setFD() 
{

	FD_ZERO(&m_fdset_socket);

	//dodaje glowny socket do zestawu m_fdset_socket
	FD_SET(m_main_socket, &m_fdset_socket);

	//dodaje sockety dla klientow do zestawu m_fdset_socket
	for (m_i = 0; m_i < m_max_clients; m_i++)
	{
		m_tmp_socket = m_client_socket[m_i];
		if (m_tmp_socket > 0)
		{
			FD_SET(m_tmp_socket, &m_fdset_socket);
		}
	}
}

int Data::isSmthActive() 
{

	//czeka na aktywnoœæ w nieskoñczonoœæ - pi¹te null
	m_activity = select(0, &m_fdset_socket, NULL, NULL, NULL);

	if (m_activity == SOCKET_ERROR)
	{
		std::cout << "Error funkcji select: " << WSAGetLastError();
		return 1;
	}
	return 0;
}

int Data::sendMsg()
{
	int sending = send(m_new_socket, m_message, strlen(m_message), 0);
	if (sending != strlen(m_message))
	{
		std::cout << "Wysylanie nie powiodlo sie";
		return 1;
	}

	std::cout << "Probne wysylanie udane\n";
	return 0;
}

void Data::addSocket()
{
	for (m_i = 0; m_i < m_max_clients; m_i++)
	{
		if (m_client_socket[m_i] == 0)
		{
			m_client_socket[m_i] = m_new_socket;
			std::cout << "Dodano do tabeli gniazd pod numerem: " << m_i << "\n";
			break;
		}
	}
}

void Data::isNewConnection() 
{
	if (FD_ISSET(m_main_socket, &m_fdset_socket))
	{
		m_new_socket = accept(m_main_socket, (struct sockaddr *)&m_address, (int *)&m_addrlen);

		isSocket(m_new_socket);
		
		//wyœwietlenie danych podlaczenia
		std::cout << "Nowe polaczenie: " << m_new_socket << ", adres ip : " 
			<< inet_ntoa(m_address.sin_addr) << ", port : " << ntohs(m_address.sin_port) << "\n";

		//wysy³anie wiadomoœci próbnej
		this->sendMsg();

		//dodawanie gniazda
		this->addSocket();

	}
}

void Data::broadcastMsg()
{
	//wypisuje wiadomosc na oknie serwera
	std::cout << inet_ntoa(m_address.sin_addr) << ":" << ntohs(m_address.sin_port) << " : " << m_buffer << "\n";
	//wysyla do wszystkich poza klientem ktory jest autorem wiadomosci
	for (m_i = 0; m_i < m_max_clients; m_i++)
	{
		if (m_client_socket[m_i] != m_tmp_socket)
		{
			send(m_client_socket[m_i], m_buffer, m_read_value, 0);
		}
	}
}

void Data::msgExchange()
{
	for (m_i = 0; m_i < m_max_clients; m_i++)
	{
		m_tmp_socket = m_client_socket[m_i];

		//jesli klient jest dodany           
		if (FD_ISSET(m_tmp_socket, &m_fdset_socket))
		{
			//pobiera dane od obiektu przychodzacego i przypisuje do m_adress
			getpeername(m_tmp_socket, (struct sockaddr*)&m_address, (int*)&m_addrlen);

			//odczytanie przychodzacego sygnalu
			m_read_value = recv(m_tmp_socket, m_buffer, m_buffsize, 0);


			if (m_read_value == SOCKET_ERROR)
			{
				int error_code = WSAGetLastError();

				//jesli ktos sie odlaczyl
				if (error_code == WSAECONNRESET)
				{
					
					std::cout <<"Klient odlaczyl sie , adres ip " << inet_ntoa(m_address.sin_addr) 
						<< ", port " << ntohs(m_address.sin_port) << "\n";

					
					closesocket(m_tmp_socket);
					m_client_socket[m_i] = 0;
				}
				else
				{
					std::cout << "Blad podczas odbierania : " << error_code;
				}
			}
			if (m_read_value == 0)
			{
				std::cout << "Klient odlaczony , adres ip " << inet_ntoa(m_address.sin_addr)
					<< ", port " << ntohs(m_address.sin_port) << "\n";

				closesocket(m_tmp_socket);
				m_client_socket[m_i] = 0;
			}
			else
				this->broadcastMsg();
		}
	}
}

SOCKET Data::getSocket()
{
	return m_tmp_socket;
}

class Ending
{
public:
	bool isEnd;
	Ending() : isEnd(false)
	{
	}

};

void waitForEsc(void * mainData)
{
	Ending *someData = reinterpret_cast<Ending*>(mainData);
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
