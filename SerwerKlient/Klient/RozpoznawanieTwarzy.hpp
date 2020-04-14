#ifndef ROZPOZNAWANIE_TWARZY
#define ROZPOZNAWANIE_TWARZY

#include <iostream>
#include <SFML/Network.hpp>
#include "File.hpp"

using namespace std;

namespace Rozpoznawanie
{
	void Program(const char* IP, long port, sf::TcpSocket& gniazdo, const char* nazwa_portretu)
	{
		// Wczytywanie portretu i wysylka
		File portret = ReadFile(nazwa_portretu);
		{ sf::Packet pak; WriteFileToPacket(pak,portret); gniazdo.send(pak); }
		
		// Tymczasowe zerwanie polaczenia
		gniazdo.disconnect();
		
		// Oczekiwanie na ponowne polaczenie
		while(gniazdo.connect(IP,port) != sf::Socket::Done) { delay(5); }
				
	}
}

#endif
