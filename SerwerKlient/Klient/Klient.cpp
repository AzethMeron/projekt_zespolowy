#include <iostream>
#include <SFML/Network.hpp>
#include <string>
#include <fstream>
#include <chrono>
#include <thread>

void delay(const int& ms)
{
	std::chrono::duration<int, std::milli> timespan(ms);
	std::this_thread::sleep_for(timespan);
}

#include "File.hpp"
#include "RozpoznawanieTwarzy.hpp"

#define PORT_CON 32000
#define PORT_ERR 0

using namespace std;

uint32_t PolaczPobierzPort(const string& IP)
{
	sf::TcpSocket gniazdo;
	if(gniazdo.connect(IP,PORT_CON) != sf::Socket::Done)
	{
		cout << "Blad polaczenia z portem: " << PORT_CON << endl;
		return PORT_ERR;
	}
	sf::Packet pak;
	gniazdo.receive(pak);
	uint32_t wyjscie;
	pak >> wyjscie;
	return wyjscie;
}

void PolaczZWatkiem(sf::TcpSocket& gniazdo, const string& IP, const uint32_t& port)
{
	while( gniazdo.connect(IP,port) != sf::Socket::Done )
	{
		cout << "Blad polaczenia z portem: " << port << endl;
		delay(50);
		cout << "Probuje ponownie" << endl;
	}
	cout << "Polaczenie z portem: " << port  << " udane" <<endl;
}

int main(int argc, char **argv)
{
	if(argc != 3)
	{
		cout << "Blad wywolania programu. Oczekiwano:" << endl;
		cout << "./nazwa_programu argument1 argument2" << endl;
		cout << "argument1 - adres IP serwera" << endl;
		cout << "argument2 - nazwa pliku z portretem pamieciowym" << endl;
		return -10;
	}
	
	uint32_t port = PolaczPobierzPort(argv[1]);
	if(port == PORT_ERR)
	{
		cout << "Blad polaczenia na porcie: " << PORT_CON << endl;
		return -1;
	}
	
	sf::TcpSocket gniazdo;
	PolaczZWatkiem(gniazdo,argv[1],port);
	
	Rozpoznawanie::Program(argv[1],port,gniazdo,argv[2]);
	
	return 0;
}
