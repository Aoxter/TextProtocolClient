#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include "pch.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <ctime>
#include <clocale>
#include <random>
#include <limits.h>
#include <vector>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 120

class message {
private:
	std::string date;      //g->dzien-miesiac-rok godzina:minuta#
	std::string operation; //o->operacja# : nawiazanie_polaczenia; zakonczenie_polaczenia; potwierdzenie; losowanie; modulo; mnozenie; dzielenie_calkowite; sortowanie_rosnaco; sortowanie_malejaco
	std::string status;    //s->status#: brak_bledu; niepoprawny_argument; wynik_poza_zakresem; wewnetrzny_blad_serwera
	std::string id;        //i->id# : id = int;
	std::string arg1;      //a->value# value = int
	std::string arg2;      //a->value# value = int
	std::string last;      //ostatni->bool# bool = {tak; nie}
public:
	std::string message_str;
	char date_value[17];   //dzien-miesiac-rok godzina:minuta
	std::string oper_value;
	std::string stat_value;
	int id_value;          //value of id
	int arg1_value;        //value of arg
	int arg2_value;        //value of arg
	bool last_value;

	message() {}
	message(std::string recv) {
		date.clear();
		operation.clear();
		status.clear();
		id.clear();
		arg1.clear();
		arg2.clear();
		last.clear();
		std::string temp_str;
		int temp = 0;             //wystapienia #, identyfikuje pola
		//przegladanie calej wiadomosci, rozpoznawanie pol
		for (int i = 0; i < recv.size(); i++) {
			//czesc odpowiadajaca polom naglowka
			if (temp <= 3) {
				switch (temp) {
					//pole daty
				case 0: {
					//znak # oznacza koniec pola
					if (recv[i] == '#') {
						//dopisywanie do zmiennej znakow # i NULL
						temp_str = "#";
						date += temp_str;
						temp++;
					}
					//przepisywanie wartosci pola do wystapienia znaku #
					else date += recv[i];
					break;
				}
						//pole operacji
				case 1: {
					if (recv[i] == '#') {
						temp_str = "#";
						operation += temp_str;
						temp++;
					}
					else operation += recv[i];
					break;
				}
						//pole statusu
				case 2: {
					if (recv[i] == '#') {
						temp_str = "#";
						status += temp_str;
						temp++;
					}
					else status += recv[i];
					break;
				}
						//pole identyfikatora
				case 3: {
					if (recv[i] == '#') {
						temp_str = "#";
						id += temp_str;
						temp++;
					}
					else id += recv[i];
					break;
				}
				}
			}
			//czesc odpowiadajaca rescie pol
			else {
				//wystapil blad podczas wykonywania operacji
				if (status == "s->brak_bledu#") {
					//operacje z dwoma polami argumentow
					if ((operation == "o->losowanie#") || (operation == "o->modulo#") || (operation == "o->mnozenie#")
						|| (operation == "o->dzielenie_calkowite#")) {
						switch (temp) {
							//pole argumentu
						case 4: {
							if (recv[i] == '#') {
								temp_str = "#";
								arg1 += temp_str;
								temp++;
							}
							else arg1 += recv[i];
							break;
						}
								//pole ostatni
						case 5: {
							if (recv[i] == '#') {
								temp_str = "#";
								last += temp_str;
								temp++;
							}
							else last += recv[i];
							break;
						}
						}
					}
					//operacje z jednym polem argumentu
					else if ((operation == "o->sortowanie_rosnaco#") || (operation == "o->sortowanie_malejaco#")) {
						switch (temp) {
							//pole argumentu
						case 4: {
							if (recv[i] == '#') {
								temp_str = "#";
								arg1 += temp_str;
								temp++;
							}
							else arg1 += recv[i];
							break;
						}
								//pole ostatni
						case 5: {
							if (recv[i] == '#') {
								temp_str = "#";
								last += temp_str;
								temp++;
							}
							else last += recv[i];
							break;
						}
						}
					}
					else {
						switch (temp) {
							//pole ostatni
						case 4: {
							if (recv[i] == '#') {
								temp_str = "#";
								last += temp_str;
								temp++;
							}
							else last += recv[i];
							break;
						}
						}
					}
				}
				//wystapil blad
			}
		}
		//ustawianie pol wartosci z pol tekstowych
		if ((operation == "o->sortowanie_rosnaco#") || (operation == "o->sortowanie_malejaco#")) {
			std::string temp_str;
			//pole daty
			for (int i = 0; i < 16; i++) date_value[i] = date[i + 3];
			//pole operacji
			oper_value.clear();
			for (int i = 3; i < operation.size() - 1; i++) oper_value += operation[i];
			//pole statusu
			stat_value.clear();
			for (int i = 3; i < status.size() - 1; i++) stat_value += status[i];
			//pole id
			temp_str.clear();
			for (int i = 3; i < id.size() - 1; i++) temp_str += id[i];
			id_value = stoi(temp_str);
			//pole argumentu
			temp_str.clear();
			for (int i = 3; i < arg1.size() - 1; i++) temp_str += arg1[i];
			arg1_value = stoi(temp_str);
			//pole ostatni
			if (last == "ostatni->nie#") last_value = false;
			else last_value = true;
		}
		else if (operation == "o->potwierdzenie#") {
			std::string temp_str;
			//pole daty
			for (int i = 0; i < 16; i++) date_value[i] = date[i + 3];
			//pole operacji
			oper_value.clear();
			for (int i = 3; i < operation.size() - 1; i++) oper_value += operation[i];
			//pole statusu
			stat_value.clear();
			for (int i = 3; i < status.size() - 1; i++) stat_value += status[i];
			//pole id
			temp_str.clear();
			for (int i = 3; i < id.size() - 1; i++) temp_str += id[i];
			id_value = stoi(temp_str);
		}
		else {
			std::string temp_str;
			//pole daty
			for (int i = 0; i < 16; i++) date_value[i] = date[i + 3];
			//pole operacji
			oper_value.clear();
			for (int i = 3; i < operation.size() - 1; i++) oper_value += operation[i];
			//pole statusu
			stat_value.clear();
			for (int i = 3; i < status.size() - 1; i++) stat_value += status[i];
			//pole id
			temp_str.clear();
			for (int i = 3; i < id.size() - 1; i++) temp_str += id[i];
			id_value = stoi(temp_str);
			//pole argumentu
			temp_str.clear();
			for (int i = 3; i < arg1.size() - 1; i++) temp_str += arg1[i];
			arg1_value = stoi(temp_str);
			//pole ostatni
			if (last == "ostatni->nie#") last_value = false;
			else last_value = true;
		}
	}
	~message() {}

	void get_current_time() {
		time_t czas;
		time(&czas);
		tm czasTM;
		localtime_s(&czasTM, &czas);
		setlocale(LC_ALL, "Polish");
		strftime(date_value, sizeof(date_value), "%d-%m-%Y %H:%M", &czasTM);
	}

	void pack_to_string() {
		//ustawianie pol tekstowych z pol wartosci
		std::string temp_str;
		//pole daty
		date = "g->";
		for (int i = 0; i < 16; i++) date += date_value[i];
		date += "#";
		message_str = date;
		//pole operacji
		operation = "o->" + oper_value + "#";
		//pole statusu
		status = "s->" + stat_value + "#";
		//pole id
		id = "i->" + std::to_string(id_value) + "#";
		if ((oper_value == "potwierdzenie") || (oper_value == "nawiazanie_polaczenia") ||
			(oper_value == "zakonczenie_polaczenia")) {
			//pole ostatni
			if (last_value) last = "ostatni->tak#";
			else last = "ostatni->nie#";
			//pakowanie pol tekstowych do jednego string'a
			message_str = message_str + operation + status + id + last;
		}
		else if ((oper_value == "sortowanie_rosnaco") || (oper_value == "sortowanie_malejaco")) {
			//pole argumentu
			arg1 = "a->" + std::to_string(arg1_value) + "#";
			//pole ostatni
			if (last_value) last = "ostatni->tak#";
			else last = "ostatni->nie#";
			//pakowanie pol tekstowych do jednego string'a
			message_str = message_str + operation + status + id + arg1 + last;
		}
		else {
			//pole argumentu
			arg1 = "a->" + std::to_string(arg1_value) + "#";
			//pole argumentu
			arg2 = "a->" + std::to_string(arg2_value) + "#";
			//pole ostatni
			if (last_value) last = "ostatni->tak#";
			else last = "ostatni->nie#";
			//pakowanie pol tekstowych do jednego string'a
			message_str = message_str + operation + status + id + arg1 + arg2 + last;
		}
	}

};

int user_inteface() {
	int option;
	std::cout << "Co chcesz zrobic?\n0 - zakonczenie polaczenia\n1 - losowanie liczby z zadanego przedzialu\n"
		"2 - operacja modulo\n3 - mnozenie\n4 - dzielenie calkowite\n5 - posortowac liczby rosnaco\n6 - posortowac liczby malejaco\n";
	std::cin >> option;
	return option;
}

int main()
{
	WSADATA wsaData;
	SOCKET ConnectSocket = INVALID_SOCKET;
	int iResult;
	int session_id = 0;
	std::string recv_str;

	std::string ip_addr;

	std::cout << "Podaj adres IP serwera" << std::endl;
	std::cin >> ip_addr;
	const char* ip = ip_addr.c_str();
	//const char* ip = "127.0.0.1";        
	ULONG* IPaddr = new ULONG;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	//Create a Socket for connecting to server
	ConnectSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (ConnectSocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(1234);
	inet_pton(AF_INET, ip, IPaddr);
	addr.sin_addr.s_addr = *IPaddr;

	char buffer[DEFAULT_BUFLEN];
	int len = sizeof(sockaddr_in);

	//wysylanie zadania nawiazania polaczenia
	message* request = new message();
	request->get_current_time();
	request->oper_value = "nawiazanie_polaczenia";
	request->stat_value = "brak_bledu";
	request->id_value = session_id;
	request->last_value = true;
	request->pack_to_string();
	strcpy_s(buffer, request->message_str.c_str());
	iResult = sendto(ConnectSocket, buffer, request->message_str.size(), 0, (sockaddr*)&addr, len);
	if (iResult == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}
	//oczekiwanie na potwierdzenie odbioru
	iResult = recvfrom(ConnectSocket, buffer, DEFAULT_BUFLEN, 0, (sockaddr*)&addr, &len);
	if (iResult == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}
	recv_str = buffer;
	message* answer = new message(recv_str);
	session_id = answer->id_value;
	std::cout << "Otrzymano zadanie nawiazania polaczenia z godziny: ";
	for (int i = 0; i < 16; i++) {
		std::cout << answer->date_value[i];
	}
	std::cout << std::endl;

	bool end = false;
	do {
		int option;
		option = user_inteface();
		if (option == 0) {
			//wysylanie zadania zakonczenia polaczenia
			request = new message();
			request->get_current_time();
			request->oper_value = "zakonczenie_polaczenia";
			request->stat_value = "brak_bledu";
			request->id_value = session_id;
			request->last_value = true;
			request->pack_to_string();
			strcpy_s(buffer, request->message_str.c_str());
			iResult = sendto(ConnectSocket, buffer, request->message_str.size(), 0, (sockaddr*)&addr, len);
			if (iResult == SOCKET_ERROR) {
				printf("send failed with error: %d\n", WSAGetLastError());
				closesocket(ConnectSocket);
				WSACleanup();
				return 1;
			}
			end = true;
		}

		else if ((option > 0) && (option < 5)) {
			//wysylanie zadania operacji dwuargumentowej
			request = new message();
			if (option == 1) {
				request->oper_value = "losowanie";
				std::cout << "Podaj poczatek zakresu losowania" << std::endl;
				std::cin >> request->arg1_value;
				std::cout << "Podaj koniec zakresu losowania" << std::endl;
				std::cin >> request->arg2_value;
			}
			else if (option == 2) {
				request->oper_value = "modulo";
				std::cout << "Podaj pierwszy argument operacji modulo" << std::endl;
				std::cin >> request->arg1_value;
				std::cout << "Podaj drugi argument operacji modulo" << std::endl;
				std::cin >> request->arg2_value;
			}
			else if (option == 3) {
				request->oper_value = "mnozenie";
				std::cout << "Podaj pierwszy argument mnozenia" << std::endl;
				std::cin >> request->arg1_value;
				std::cout << "Podaj drugi argument mnozenia" << std::endl;
				std::cin >> request->arg2_value;
			}
			else if (option == 4) {
				request->oper_value = "dzielenie_calkowite";
				std::cout << "Podaj pierwszy argument dzielenia" << std::endl;
				std::cin >> request->arg1_value;
				std::cout << "Podaj drugi argument dzielenia" << std::endl;
				std::cin >> request->arg2_value;
			}
			request->get_current_time();
			request->stat_value = "brak_bledu";
			request->id_value = session_id;
			request->last_value = true;
			request->pack_to_string();
			strcpy_s(buffer, request->message_str.c_str());
			strcpy_s(buffer, request->message_str.c_str());
			iResult = sendto(ConnectSocket, buffer, request->message_str.size(), 0, (sockaddr*)&addr, len);
			if (iResult == SOCKET_ERROR) {
				printf("send failed with error: %d\n", WSAGetLastError());
				closesocket(ConnectSocket);
				WSACleanup();
				return 1;
			}
		}

		else if ((option == 5) || (option == 6)) {
			//wysylanie zadania operacji sortowania
			std::vector<int> numbers;
			bool temp_bool = true;
			int temp_int;
			//wysylanie liczb do posortowania
			do {
				request = new message();
				if (option == 5) request->oper_value = "sortowanie_rosnaco";
				else request->oper_value = "sortowanie_malejaco";
				std::cout << "Podaj liczbe" << std::endl;
				std::cin >> request->arg1_value;
				std::cout << "Ostatnia?\n0 - nie\n1 - tak" << std::endl;
				std::cin >> temp_int;
				if (temp_int == 1) {
					request->last_value = true;
					temp_bool = false;
				}
				else if (temp_int == 0) request->last_value = false;
				request->stat_value = "brak_bledu";
				request->id_value = session_id;
				request->get_current_time();
				request->pack_to_string();
				strcpy_s(buffer, request->message_str.c_str());
				iResult = sendto(ConnectSocket, buffer, request->message_str.size(), 0, (sockaddr*)&addr, len);
				if (iResult == SOCKET_ERROR) {
					printf("send failed with error: %d\n", WSAGetLastError());
					closesocket(ConnectSocket);
					WSACleanup();
					return 1;
				}
				//oczekiwanie na potwierdzenie odbioru
				iResult = recvfrom(ConnectSocket, buffer, DEFAULT_BUFLEN, 0, (sockaddr*)&addr, &len);
				if (iResult == SOCKET_ERROR) {
					printf("send failed with error: %d\n", WSAGetLastError());
					closesocket(ConnectSocket);
					WSACleanup();
					return 1;
				}
				recv_str = buffer;
				answer = new message(recv_str);
				std::cout << "Otrzymano potwierdzenie odbioru" << std::endl;
			} while (temp_bool);

			temp_bool = true;
			//odbieranie posortowanych liczb
			do {
				//oczekiwanie na liczbe
				iResult = recvfrom(ConnectSocket, buffer, DEFAULT_BUFLEN, 0, (sockaddr*)&addr, &len);
				if (iResult == SOCKET_ERROR) {
					printf("send failed with error: %d\n", WSAGetLastError());
					closesocket(ConnectSocket);
					WSACleanup();
					return 1;
				}
				recv_str = buffer;
				answer = new message(recv_str);
				numbers.push_back(answer->arg1_value);
				if (answer->last_value) temp_bool = false;
				//potwierdzanie odbioru liczby
				request = new message();
				request->get_current_time();
				request->oper_value = "potwierdzenie";
				request->stat_value = "brak_bledu";
				request->id_value = session_id;
				request->last_value = true;
				request->pack_to_string();
				strcpy_s(buffer, request->message_str.c_str());
				iResult = sendto(ConnectSocket, buffer, request->message_str.size(), 0, (sockaddr*)&addr, len);
				if (iResult == SOCKET_ERROR) {
					printf("send failed with error: %d\n", WSAGetLastError());
					closesocket(ConnectSocket);
					WSACleanup();
					return 1;
				}
			} while (temp_bool);
			std::cout << "Posortowane liczby:\n";
			for (int i : numbers) std::cout << i << "; ";
			std::cout << std::endl;
		}

		if ((answer->oper_value != "sortowanie_rosnaco") && (answer->oper_value != "sortowanie_malejaco")) {
			//oczekiwanie potwierdzenia odbioru
			iResult = recvfrom(ConnectSocket, buffer, DEFAULT_BUFLEN, 0, (sockaddr*)&addr, &len);
			if (iResult == SOCKET_ERROR) {
				printf("send failed with error: %d\n", WSAGetLastError());
				closesocket(ConnectSocket);
				WSACleanup();
				return 1;
			}
			recv_str = buffer;
			answer = new message(recv_str);
			if (answer->oper_value == "potwierdzenie") std::cout << "Otrzymano potwierdzenie odbioru" << std::endl;
			//oczekiwanie wyniku
			if (request->oper_value != "zakonczenie_polaczenia") {
				iResult = recvfrom(ConnectSocket, buffer, DEFAULT_BUFLEN, 0, (sockaddr*)&addr, &len);
				if (iResult == SOCKET_ERROR) {
					printf("send failed with error: %d\n", WSAGetLastError());
					closesocket(ConnectSocket);
					WSACleanup();
					return 1;
				}
				recv_str = buffer;
				answer = new message(recv_str);
				if (answer->stat_value == "brak_bledu") {
					std::cout << "Otrzymano wynik: " << answer->arg1_value << std::endl;
				}
				else {
					if (answer->stat_value == "niepoprawny_argument") {
						std::cout << "Podany zostal niepoprawny argument, operacja anulowana\n";
					}
					else if (answer->stat_value == "wynik_poza_zakresem") {
						std::cout << "Wynik operacji wykracza poza zakres typu, operacja anulowana\n";
					}
					else if (answer->stat_value == "wewnetrzny_blad_serwera") {
						std::cout << "Wystapil wewnetrzny blad po stronie serwera, operacja anulowana\n";
					}
				}
				//wysylanie potwierdzenia odbioru wyniku
				request = new message();
				request->get_current_time();
				request->oper_value = "potwierdzenie";
				request->stat_value = "brak_bledu";
				request->id_value = session_id;
				request->last_value = true;
				request->pack_to_string();
				strcpy_s(buffer, request->message_str.c_str());
				iResult = sendto(ConnectSocket, buffer, request->message_str.size(), 0, (sockaddr*)&addr, len);
				if (iResult == SOCKET_ERROR) {
					printf("send failed with error: %d\n", WSAGetLastError());
					closesocket(ConnectSocket);
					WSACleanup();
					return 1;
				}
			}
		}
	} while (!end);

	printf("Connection closed\n");

	// cleanup
	closesocket(ConnectSocket);
	WSACleanup();

	return 0;
}