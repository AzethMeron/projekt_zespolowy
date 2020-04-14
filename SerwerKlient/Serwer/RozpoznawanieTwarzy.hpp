#ifndef ROZPOZNAWANIE_TWARZY
#define ROZPOZNAWANIE_TWARZY

#include <iostream>
#include <SFML/Network.hpp>
#include "Dir.hpp"
#include "File.hpp"
#include "BazaUniwersalna.hpp"
#include <vector>
#include <string>
#include <fstream>
#include <pthread.h>

using namespace std;

namespace Rozpoznawanie
{

/* ********************************************************************************* */

	class Informacje
	{
		protected:
			string imie;
			string nazwisko;
			string pesel;
			string notatki;
		public:
			string Imie(void) const { return imie; }
			string Nazwisko(void) const { return nazwisko; }
			string Pesel(void) const { return pesel; }
			string Notatki(void) const { return notatki; }
			void Wczytaj(istream& wejscie) 
			{
				// tymczasowosc
				wejscie >> imie >> nazwisko >> pesel >> notatki;
			}
	};

	ostream& operator << (ostream& wyjscie, const Informacje& info)
	{
		wyjscie << info.Imie() << ' ' << info.Nazwisko() << ' ' << info.Pesel() << ' ' << info.Notatki(); 
		return wyjscie;
	}

	struct Osoba
	{
		vector<string> zdjecia; // tymczasowo, ostatecznie powinien byc wektor kodow zdjec
		Informacje dane; 
	};

	ostream& operator << (ostream& wyjscie, const Osoba& ob)
	{
		wyjscie << ob.dane << endl;
		for(unsigned int i = 0; i < ob.zdjecia.size(); ++i)
		{
			wyjscie << ob.zdjecia[i] << endl;
		}
		return wyjscie;
	}

/* ********************************************************************************* */

	class BazaOsob : public BazaUniwersalna<Osoba>
	{
		protected:
			Osoba WczytajWpis(const string& dirname) const override
			{
				ifstream plik_dane(dirname+"dane.txt");
				if(!plik_dane.is_open()) { throw false; }
				vector<string> listaPlikow = GetFilesInDir(dirname.c_str());
				RemoveFileFromList(listaPlikow,"dane.txt");
				
				Osoba output;
				for(unsigned int i = 0; i < listaPlikow.size(); ++i)
				{
					output.zdjecia.push_back(dirname+listaPlikow[i]);
				}
				output.dane.Wczytaj(plik_dane);
				return output;
			}
		public:
			BazaOsob(const string& dir)
			{
				ofstream logs ("tmp/logs/Baza.txt");
				WczytajBaze(dir);
				WypiszLogi(logs);
			}
			void WczytajBaze(const string& dirname) override;
	};

/* ********************************************************************************* */

	pthread_mutex_t baza_globalna_mutex;
	BazaOsob baza_globalna("BazaDanych/");

	void BazaOsob::WczytajBaze(const string& dirname)
	{
		pthread_mutex_lock(&baza_globalna_mutex);
		BazaUniwersalna::WczytajBaze(dirname);
		pthread_mutex_unlock(&baza_globalna_mutex);
	}

/* ********************************************************************************* */

	void Program(sf::TcpListener& sluchacz, long port, sf::TcpSocket& gniazdo, ostream& logs)
	{
		// Pobieranie zdjecia od klienta
		sf::Packet pak; 
		gniazdo.receive(pak);
		File portret = ReadFileFromPacket(pak);
		string sciezka_portretu;
		{ char temp[50]; sprintf(temp,"tmp/img/%ld",port); string sciezka_portretu = temp; portret.Save(sciezka_portretu); }
		
		// Tymczasowe zrywanie polaczenia
		gniazdo.disconnect();
		
		// Pobieranie bazy zdjec 
		pthread_mutex_lock(&baza_globalna_mutex);
		const BazaOsob baza_lokalna = baza_globalna;
		pthread_mutex_unlock(&baza_globalna_mutex);
		
		// Praca algorytmu - lista najlepszej zgodności
		
		// Ponowne polaczenie
		sluchacz.accept(gniazdo);
		
		// Wysylanie wynikow
	}
}

#endif
