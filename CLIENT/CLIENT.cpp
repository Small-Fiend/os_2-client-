// CLIENT.cpp: определяет точку входа для консольного приложения.
//

// Пример простого TCP клиента
#include "stdafx.h"
#include < iostream >
#pragma comment( lib, "ws2_32.lib" )
#include < Windows.h >
#include < conio.h >
#include <string>

using namespace std;

#define PORT 9090
#define SERVERADDR "127.0.0.1"

DWORD WINAPI GetNewMessage(LPVOID client_socket);


string encript(string text, int key) {
	string newWord = "";
	for (char ch : text) {
		if (!isalpha(ch)) {
			newWord += ch;
			continue;
		}
		else if (ch >= 'A' && ch <= 'Z') {
			ch += (key % 26);
			if (ch > 'Z')
				ch -= 26;
			newWord += ch;
		}
		else if (ch >= 'a' && ch <= 'z') {
			ch += (key % 26);
			if (ch > 'z')
				ch -= 26;
			newWord += ch;
		}
	}
	return newWord;
}

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "Rus");
	string a;
	    int key;
	
	    string b;
	    cout << "Введите строку которую хотите зашифровать: " << endl;
	    getline(cin, a);
	    cout << "Введите ключ: " << endl;
	    cin >> key;
	    cout << encript(a, key) << endl;

	char buff[1024];
	printf("TCP CLIENT\n");

	// инициализация библиотеки Winsock
	if (WSAStartup(0x202, (WSADATA *)&buff[0]))
	{
		printf("WSAStart error %d\n", WSAGetLastError());
		return -1;
	}

	// создание сокета
	SOCKET my_sock;
	my_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (my_sock < 0)
	{
		printf("Socket() error %d\n", WSAGetLastError());
		return -1;
	}

	// Шаг 3 - установка соединения

	// заполнение структуры sockaddr_in
	// указание адреса и порта сервера
	sockaddr_in dest_addr;
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(PORT);
	HOSTENT *hst;

	// преобразование IP адреса из символьного в сетевой формат
	if (inet_addr(SERVERADDR) != INADDR_NONE)
		dest_addr.sin_addr.s_addr = inet_addr(SERVERADDR);
	else
		// попытка получить IP адрес по доменному имени сервера
		if (hst = gethostbyname(SERVERADDR))
			// hst->h_addr_list содержит не массив адресов,  а массив указателей на адреса
			((unsigned long *)&dest_addr.sin_addr)[0] =
			((unsigned long **)hst->h_addr_list)[0][0];
		else
		{
			printf("Invalid address %s\n", SERVERADDR);
			closesocket(my_sock);
			WSACleanup();
			return -1;
		}

	// адрес сервера получен – пытаемся установить  соединение 
	if (connect(my_sock, (sockaddr *)&dest_addr,
		sizeof(dest_addr)))
	{
		printf("Connect error %d\n", WSAGetLastError());
		return -1;
	}

	printf("connection with %s successfully installed\n\
    Type quit for quit\n\n", SERVERADDR);

	DWORD thID;
	//CreateThread(NULL, NULL, GetNewMessage, &my_sock, NULL, &thID);

	// чтение и передача сообщений
	int nsize;
	while ((nsize = recv(my_sock, &buff[0], sizeof(buff) - 1, 0)) != SOCKET_ERROR)
	{
		// ставим завершающий ноль в конце строки 
		buff[nsize] = 0;

		// выводим на экран 
		printf("S=>C:%s", buff);

		// читаем пользовательский ввод с клавиатуры
		printf("S<=C:"); fgets(&buff[0], sizeof(buff) - 1, stdin);

		// проверка на "quit"
		if (!strcmp(&buff[0], "quit\n"))
		{
			// Корректный выход
			printf("Exit...");
			closesocket(my_sock);
			WSACleanup();
			return 0;
		}

		// передаем строку клиента серверу
		send(my_sock, &buff[0], nsize, 0);
	}
	printf(buff);
	printf("Recv error %d\n", WSAGetLastError());
	closesocket(my_sock);
	WSACleanup();
	system("pause");
	return -1;
}

/* эту функцию надо использовать если запущена
CheckCommonBuffer() на сервере*/
DWORD WINAPI GetNewMessage(LPVOID client_socket)
{

	SOCKET my_sock;
	my_sock = ((SOCKET *)client_socket)[0];
	int nsize;
	char buff[1024];

	while ((nsize = recv(my_sock, &buff[0], sizeof(buff) - 1, 0)) != SOCKET_ERROR)
	{

		// ставим завершающий ноль в конце строки 
		buff[nsize] = 0;

		// выводим на экран 
		printf("S=>C:%s", buff);
	}
	system("pause");
	return 0;
}