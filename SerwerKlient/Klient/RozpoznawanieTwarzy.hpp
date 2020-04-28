#ifndef ROZPOZNAWANIE_TWARZY
#define ROZPOZNAWANIE_TWARZY

#include <iostream>
#include <SFML/Network.hpp>
#include "File.hpp"
#include <string>
#include <algorithm>

using namespace std;

struct Wynik
{
	File informacje;
	double zgodnosc;
	Wynik(const double& z, const File& f) : informacje(f), zgodnosc(z) {}
};

vector<Wynik> RozpakujWynik(sf::Packet& pak)
{
	uint32_t size;
	pak >> size;
	vector<Wynik> wynik;
	for(unsigned int i = 0; i < size; ++i)
	{
		double zgodnosc;
		pak >> zgodnosc; 
		Wynik tmp((double)zgodnosc,ReadFileFromPacket(pak));
		wynik.push_back(tmp);
	}
	return wynik;
}

vector<string> WyciagnijInformacje(const File& plik)
{
	vector<string> wyjscie;
	vector<char> temp_vessel;
	for(unsigned int i = 0; i < plik.Length; ++i)
	{
		if(plik[i] == '\n' || i == plik.Length - 1)
		{
			string conv(temp_vessel.begin(),temp_vessel.end());
			wyjscie.push_back(conv);
			temp_vessel.clear();
		}
		else
		{
			temp_vessel.push_back(plik[i]);
		}
	}
	return wyjscie;
}

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
		
		// Pobieranie wyniku
		sf::Packet wynik;
		gniazdo.receive(wynik);
		vector<Wynik> lista = RozpakujWynik(wynik);
		
		// Wyswietlanie wyniku
		cout << "Wyniki. Im liczba mniejsza tym lepiej" << endl;
		for(unsigned int i = 0; i < lista.size(); ++i)
		{
			cout << "- " << lista[i].zgodnosc;
			vector<string> informacje = WyciagnijInformacje(lista[i].informacje);
			unsigned int elementy_na_pasku = min((unsigned int)3,(unsigned int)informacje.size());
			for(unsigned j = 0; j < elementy_na_pasku; ++j)
			{
				cout << ' ' << informacje[j];
			}
			cout << endl;
		}
		
		// Papa
		gniazdo.disconnect();
	}
}

#endif
