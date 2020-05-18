#ifndef ROZPOZNAWANIE_TWARZY
#define ROZPOZNAWANIE_TWARZY

#include <iostream>
#include <cstdio>
#include <SFML/Network.hpp>
#include "Dir.hpp"
#include "File.hpp"
#include "BazaUniwersalna.hpp"
#include <vector>
#include <string>
#include <fstream>
#include <pthread.h>
#include <numeric>
#include "Python.hpp"

using namespace std;

namespace Rozpoznawanie
{

/* ********************************************************************************* */

struct Osoba
{
	public:
		vector<double> zgodnosc;
		string katalog;
		
		inline double Srednia(void) const
		{
			double wynik = 0;
			for(unsigned int i = 0; i < zgodnosc.size(); ++i)
			{
				wynik += zgodnosc[i];
			}
			return wynik/(double)zgodnosc.size();
		}
		
		inline File Informacje(void) const
		{
			return ReadFile("BazaDanych/"+katalog+"/info.txt");
		}
};

bool PorownajOsoby(const Osoba& l, const Osoba& p)
{
	return l.Srednia() < p.Srednia();
}

struct Wyniki
{
	protected:
		vector<Osoba> wyniki;
		int Znajdz(const string& nazwa) const
		{
			// Sprawdz ostatni dodany - dosc czesty przypadek
			if(wyniki.size() > 0) if(wyniki.back().katalog == nazwa) return (wyniki.size()-1);
			// Glowna petla
			for(unsigned int i = 0; i < wyniki.size(); ++i)
			{
				if(wyniki[i].katalog == nazwa)
					return (int)i;
			}
			// Jesli nie znaleziono
			return -1;
		}
	public:
		void Wczytaj(istream& wejscie)
		{
			{ string tmp; getline(wejscie,tmp); }
			while(wejscie.good())
			{
				double zgodnosc = -1;
				string katalog = "placeholder";
				
				string tmp; 
				getline(wejscie,tmp); 
				if(!wejscie.good()) break;
				zgodnosc = stod(tmp); 
				
				getline(wejscie,katalog);
				if(!wejscie.good()) break;
						
				int ind = Znajdz(katalog);
				if(ind == -1)
				{
					wyniki.emplace_back();
					wyniki.back().katalog = katalog;
					wyniki.back().zgodnosc.push_back(zgodnosc);
				}
				else
				{
					wyniki[ind].zgodnosc.push_back(zgodnosc);
				}
			}
		}
		vector<Osoba> PobierzListe(const unsigned int& MaxNum)
		{
			sort(wyniki.begin(),wyniki.end(),PorownajOsoby);
			vector<Osoba> wyjscie;
			for(unsigned int i = 0; i < wyniki.size(); ++i)
			{
				if(i < MaxNum)
				{
					wyjscie.push_back(wyniki[i]);
				}
			}
			return wyjscie;
		}
};

void PakujWyniki(sf::Packet& pak, vector<Osoba> lista)
{
	pak << (uint32_t)lista.size();
	for(unsigned int i = 0; i < lista.size(); ++i)
	{
		pak << (double_t)lista[i].Srednia();
		WriteFileToPacket(pak,lista[i].Informacje());
	}
}

/* ********************************************************************************* */

	void Calc(const string& portret, const string& plik_wynikowy)
	{
		vector<string> tmp;
		tmp.push_back("-i");
		tmp.push_back(portret);
		tmp.push_back("-e");
		tmp.push_back("tmp/facial_encodings.pkl");
		tmp.push_back("-f");
		tmp.push_back(plik_wynikowy);
		CallScript("Python/calculations.py",tmp);
	}
	
	void Setup(void)
	{
		vector<string> tmp;
		tmp.push_back("-d");
		tmp.push_back("BazaDanych");
		tmp.push_back("-e");
		tmp.push_back("tmp/facial_encodings.pkl");
		CallScript("Python/encodings.py",tmp);
	}

	void Program(sf::TcpListener& sluchacz, long port, sf::TcpSocket& gniazdo, ostream& logs)
	{
		// Pobieranie zdjecia od klienta
		sf::Packet pak; 
		gniazdo.receive(pak);
		File portret = ReadFileFromPacket(pak);
		string sciezka_portretu;
		{ char temp[50]; sprintf(temp,"tmp/img/%ld",port); sciezka_portretu = temp; portret.Save(sciezka_portretu); }
		
		// Tymczasowe zrywanie polaczenia
		gniazdo.disconnect();
		
		// Praca algorytmu - lista najlepszej zgodności
		string sciezka_wyniku;
		{ char temp[50]; sprintf(temp,"tmp/wynik/%ld",port); sciezka_wyniku = temp; }
		remove(sciezka_wyniku.c_str());
		Calc(sciezka_portretu,sciezka_wyniku);
		// Tutaj wywolanie calculations, zapis do sciezka_wyniku (plik)
		
		logs << "Koniec pracy algorytmu" << endl;
		
		// Wczytywanie wynikow
		Wyniki wyniki;
		ifstream plik_z_wynikami;
		while(!plik_z_wynikami.is_open())
		{
			plik_z_wynikami.open((sciezka_wyniku));
		}
		wyniki.Wczytaj(plik_z_wynikami);
		plik_z_wynikami.close();
		
		logs << "Zakonczono wczytywanie wynikow" << endl;
		
		// Pakowanie wynikow
		vector<Osoba> lista = wyniki.PobierzListe(2);
		logs << "Liczba elementow listy: " << lista.size() << endl;
		sf::Packet paczka_z_wynikami;
		PakujWyniki(paczka_z_wynikami,lista);
		
		logs << "Zakonczono pakowanie" << endl;
		
		// Ponowne polaczenie
		sluchacz.accept(gniazdo);
		
		// Wysylanie wynikow
		gniazdo.send(paczka_z_wynikami);
	}
}

#endif
