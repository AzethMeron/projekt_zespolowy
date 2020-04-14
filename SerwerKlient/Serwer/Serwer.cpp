#include <iostream>
#include <SFML/Network.hpp>
#include <pthread.h>
#include <list>
#include <string>
#include <fstream>
#include <chrono>
#include <thread>
#include "BazaUniwersalna.hpp"
#include "RozpoznawanieTwarzy.hpp"

#define PORT_MIN 32001
#define PORT_MAX 32100
#define PORT_CON 32000 // nie moze nalezec do <PORT_MIN,PORT_MAX>

using namespace std;

/* *********************************************************************************** */

// Flagi do budowania -lsfml-network -lsfml-system -pthread
// Wymaga C++11 lub nowszy. 
// Wymaga linuksa do pracy?
// Wymaga: libsfml-dev

// Wielowatkowosc dziala, przetestowane
// Polaczenie (lokalne) dziala, przetestowane
// Polaczenie (zdalne) nieprzetestowane

// Dzialanie programu (main-menadzer):
// - program nasluchuje na porcie PORT_CON na polaczenie (done)
// - nawiazanie polaczenia na PORT_CON (done)
// - wybiera i zajmuje jeden z wolnych portow nalezacych do <PORT_MIN,PORT_MAX> (done)
// - tworzy nowy watek i przekazuje mu wybrany port (done)
// - przeslanie do klienta numeru portu, na ktorym moze polaczyc sie z watkiem ktory moze go obsluzyc (done)
// - zerwanie polaczenia na porcie PORT_CON (done)

// Dzialanie programu (serwer):
// - Zczytanie danych przekazanych przez menadzera w postaci void* (done)
// - Nawiazanie polaczenia z klientem na przydzielonym porcie (done)
// - Test polaczenia (done)
// - (praktycznie done): Pobieranie zdjecia (portretu pamieciowego) od klienta 
// - TO DO: Obliczenie kodu otrzymanego zdjecia
// - (done) Wczytanie listy osob oraz powiazanych z nim informacji (imie, nazwisko, adnotacja, zakodowane dane na podstawie zdjec)
// - TO DO: Porownanie kodu zdjecia z elementami listy, umieszczenie informacji o osobach w kolejce priorytetowej (sortowanie po % zgodności)
// - TO DO: Wyslanie informacji o 10 najbardziej zgodnych osobach do klienta
// - Zwalnianie zasobow po zakonczeniu dzialania (done)

// Napotkane problemy:
// - TcpListener w bledny sposob zamyka nasluchiwanie na porcie (problem SFMLa) przez co ponowne rozpoczecie nasluchu na porcie to rosyjska ruleta
//   Problem naprawiono poprzez kolejna globalna liste chroniona muteksem.
// - Teoretycznie po dlugim czasie dzialania program mogl sie wysypac przez przepelnienie wektora watkow.
//   Usunalem wektor, watki sa tworzone jako detached.
// - Slaby punkt: w przypadku problemu z rozpoczeciem nasluchu na porcie X, wszystkie nastepne porty pozostana wykorzystane a program nie bedzie mogl nawiazywac nowych polaczen

// API dla aplikacji uruchamianej na serwerze.
// - sluchacz jest juz uruchomiony, nie trzeba uruchamiac nasluchiwania
// - gniazdo jest juz polaczone z odbiorca, mozna zerwac polaczenie i rozpoczac je na nowo jesli jest potrzeba
// - port na ktorym mozna sie komunikowac
// - logs to plik, do ktorego mozna zapisywac jakies informacjem
void Program(sf::TcpListener& sluchacz, long port, sf::TcpSocket& gniazdo, ostream& logs)
{
	Rozpoznawanie::Program(sluchacz,port,gniazdo,logs);
}

void delay(const int& ms)
{
	std::chrono::duration<int, std::milli> timespan(ms);
	std::this_thread::sleep_for(timespan);
}

/* *********************************************************************************** */

struct Port
{
	long port;
	bool czy_wolny;
};

pthread_mutex_t porty_mutex;// Nalezy go zajmowac w chwili przeszukiwania oraz zapisywania do. Przy odczycie z konkretnego miejsca nie trzeba
list<Port> porty_tablica;

Port& ZnajdzWolnyPort(void)
{
	for(Port& Wsk : porty_tablica)
	{
		if(Wsk.czy_wolny)
			{ return Wsk; }
	}
	return porty_tablica.back();
}

struct Sluchacz
{
	sf::TcpListener ob;
	long port;
	bool czy_wlaczony;
	
	Sluchacz() { czy_wlaczony = false; }
};

pthread_mutex_t porty_sluchacze_mutex;
list<Sluchacz> porty_sluchacze_tablica;

Sluchacz& ZnajdzSluchaczaNaPorcie(const long& port)
{
	for(Sluchacz& Wsk : porty_sluchacze_tablica)
	{
		if(Wsk.port == port)
			return Wsk;
	}
	porty_sluchacze_tablica.emplace_back();
	porty_sluchacze_tablica.back().port = port;
	return porty_sluchacze_tablica.back();
}

/* *********************************************************************************** */

void *Serwer(void* tmp)
{
	// Pobieranie argumentow wywolania
	Port* WskPort = (Port*)tmp;
	
	// Tworzenie pliku z logami:
	char logs_name[50];
	sprintf(logs_name,"tmp/logs/logs_port_%ld",WskPort->port);
	ofstream logs(logs_name);
	
	// Rozpoczecie nasluchu na przydzielonym porcie
	pthread_mutex_lock(&porty_sluchacze_mutex);
	Sluchacz& sluchacz = ZnajdzSluchaczaNaPorcie(WskPort->port);
	pthread_mutex_unlock(&porty_sluchacze_mutex);
	
	// Logi na start:
	logs << "Przydzielono zasoby: port " << WskPort->port << " oraz powiazany z nim TcpListener" << endl;
	if(sluchacz.czy_wlaczony == true)	logs << "To nie jest pierwszy raz, gdy ten port jest wykorzystywany" << endl;
	
	// Wlaczenie sluchacza
	if(sluchacz.czy_wlaczony == false)
	{
		if(sluchacz.ob.listen(WskPort->port) != sf::Socket::Done)
			{ logs << "Nieudane rozpoczecie nasluchiwania na porcie: " << WskPort->port << endl; }
		else
			{ sluchacz.czy_wlaczony = true; }
	}
	// Nawiazanie polaczenia
	if( sluchacz.czy_wlaczony == true)
	{
		logs << "Udane nasluchiwanie na porcie: " << WskPort->port << endl;
		
		// Nawiazywanie polaczenia z klientem na przydzielonym porcie
		sf::TcpSocket gniazdo;
		sluchacz.ob.accept(gniazdo);
		
		// Przejscie do wlasciwego programu - rozpoznawania twarzy
		logs << "Rozpoczecie wykonywania Programu" << endl;
		Program(sluchacz.ob, WskPort->port, gniazdo, logs);
		logs << "Zakonczono wykonywanie Programu" << endl;
	}
	
	// Zwalnianie portu, konczenie dzialania
	logs << "Konczenie pracy watku" << endl;
	pthread_mutex_lock(&porty_mutex);
	WskPort->czy_wolny = true;
	pthread_mutex_unlock(&porty_mutex);
	logs << "Zasoby zwolnione" << endl;
	logs.close();
	pthread_exit(NULL);
}

/* *********************************************************************************** */


int main()
{
	// Adres IP
	cout << endl << "Pobieranie adresu IP..." << endl;
	sf::IpAddress IP = sf::IpAddress::getPublicAddress();
	cout << "Pobrano adres IP." << endl;
	
	// Wyswietlam adres serwera
	cout << endl << "Adres IP serwera (globalny): " << IP.toString() << endl;
	cout << "Adres IP serwera (lokalny): " << sf::IpAddress::getLocalAddress().toString() << endl;
	cout << "Port do laczenia sie z programem: " << PORT_CON << endl << endl;
	
	// Kolejka portow
	for(long i = PORT_MIN; i <= PORT_MAX; ++i)
	{
		porty_tablica.emplace_back();
		porty_tablica.back().port = i;
		porty_tablica.back().czy_wolny = true;
	}
	
	// Setup sluchacza
	sf::TcpListener sluchacz;
	if(sluchacz.listen(PORT_CON) != sf::Socket::Done)
	{
		cout << "Nieudane rozpoczecie nasluchiwania na porcie: " << PORT_CON << endl;
		cout << "Jest to blad krytyczny. Dzialanie programu zakonczone. Wpisz cokolwiek, by zamknac terminal" << endl;
		char tmp; cin >> tmp; exit(-1);
	}
	
	while(true)
	{
		// Setup klienta gniazda klienta
		sf::TcpSocket gniazdo;
		// Nasluchiwanie
		cout << "Oczekiwanie na klienta..." << endl;
		sluchacz.accept(gniazdo);	
		
		// Pobieramy wolny port
		pthread_mutex_lock(&porty_mutex);
		Port* WskPort = &ZnajdzWolnyPort();
		pthread_mutex_unlock(&porty_mutex);
		if(WskPort->czy_wolny == false) cout << "Przybyl klient. Brak wolnych portow" << endl;
		while(true)
		{
			pthread_mutex_lock(&porty_mutex);
			WskPort = &ZnajdzWolnyPort();
			pthread_mutex_unlock(&porty_mutex);
			if(WskPort->czy_wolny == true) break;
			delay(500);
		}
		
		// Zajmujemy port - troche wbrew konwencji, ale nie powinno byc problemow
		WskPort->czy_wolny = false;
		cout << "Przybyl klient. Przydzielono port: " << WskPort->port << endl;
		
		// Przeslanie wybranego portu za pomoca pakietu
		sf::Packet pak;
		pak << (uint32_t)WskPort->port;
		gniazdo.send(pak);
		
		// Stworz i uruchom watek
		pthread_t watek;
		int pthread_creation_flag = pthread_create(&watek, NULL, Serwer, (void*) WskPort);
		if((pthread_creation_flag != 0) || (pthread_detach(watek))) // Sprawdzanie czy watek sie utworzyl, detachowanie
		{ cout << "Tworzenie nowego watku zakonczone niepowodzeniem. Port: " << WskPort->port << " zwolniony" << endl; WskPort->czy_wolny=true; }
		
		// Sprzatanie
		gniazdo.disconnect();
	}
	sluchacz.close();
	pthread_exit(NULL);
}

/* *********************************************************************************** */
