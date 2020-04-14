#ifndef BAZAUNIWERSALNA_HPP
#define BAZAUNIWERSALNA_HPP

#include <iostream>
#include <vector>
#include <string>
#include "Dir.hpp"

using namespace std;

// Wymagania odnosnie typu T
// operator << (ostream&, const T&)
// T(const T&)
template<typename T>
class BazaUniwersalna
{
	protected:
		vector<T> dane;
		int bledny_format;
		virtual T WczytajWpis(const string& dirname) const = 0;
	public:
		unsigned int size(void) const { return dane.size(); }
		virtual void WczytajBaze(const string& dirname)
		{
			bledny_format = 0;
			dane.clear();
			vector<string> katalog_glowny = GetFilesInDir(dirname.c_str());
			for(unsigned int i = 0; i < katalog_glowny.size(); ++i)
			{
				try { T tmp = WczytajWpis(dirname + katalog_glowny[i] + '/');
					dane.push_back(tmp); }
				catch (const bool& tmp) { ++bledny_format; }
			}
		}
		void WypiszLogi(ostream& wyjscie)
		{
			wyjscie << "Wczytano zestawow danych: " << dane.size() << endl;
			wyjscie << "Bledow formatowania: " << bledny_format << endl;
			for(unsigned int i = 0; i < dane.size(); ++i)
			{
				wyjscie << endl << dane[i];
			}
		} 
		const T& operator [] (const unsigned int& ind) const { return dane[ind]; }
};

#endif
