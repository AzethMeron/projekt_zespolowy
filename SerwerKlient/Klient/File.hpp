#ifndef FILE_HPP
#define FILE_HPP

#include <iostream>
#include <fstream>
#include <SFML/Network.hpp>
#include <functional> 
#include <string>

using namespace std;

struct File
{
	char* Bytes;
	size_t Length;
	
	char  operator [] (const int& ind) const { return Bytes[ind]; }
	char& operator [] (const int& ind)       { return Bytes[ind]; }
	
	int Save(const string& name) const
	{
		ofstream file(name);
		if(!file.good())
			return -1;
		for(unsigned int i = 0; i < Length; ++i)
		{
			file << Bytes[i];
		}
		return 0;
	}
	
	File(size_t l, char* b)
	{
		Length = l;
		Bytes = b;
	}
	
	File(const File& tmp)
	{
		Bytes = new char[tmp.Length];
		Length = tmp.Length;
		for(unsigned int i = 0; i < tmp.Length; ++i)
		{
			Bytes[i] = tmp[i];
		}
	}
	
	File(const File&& tmp)
	{
		Bytes = new char[tmp.Length];
		Length = tmp.Length;
		for(unsigned int i = 0; i < tmp.Length; ++i)
		{
			Bytes[i] = tmp[i];
		}
	}
	
	File& operator = (const File& tmp)
	{
		delete Bytes;
		Bytes = new char[tmp.Length];
		Length = tmp.Length;
		for(unsigned int i = 0; i < tmp.Length; ++i)
		{
			Bytes[i] = tmp[i];
		}
		return *this;
	}
	
	~File()
	{
		delete Bytes;
	}
	
	size_t Hash(void) const
	{
		string tmp = Bytes;
		hash<string> output;
		return output(tmp);
	}
};

File ReadFile(const string& name)  
{  
    ifstream fl(name);  
    fl.seekg( 0, ios::end );  
    size_t len = fl.tellg();  
    char *ret = new char[len];  
    fl.seekg(0, ios::beg);   
    fl.read(ret, len);  
    fl.close();  
    File output(len,ret);
    return output;  
}

File ReadFileFromPacket(sf::Packet& pak)
{
	uint32_t size;
	pak >> size;
	char* data = new char[size];
	File output(size,data);
	for(unsigned int i = 0; i < output.Length; ++i)
	{
		int8_t tmp;
		pak >> tmp;
		output[i] = tmp;
	}
	return output;
}

// Wypadaloby zrobic dzielenie pliku na pakiety, test, hasze, czy odebrano itd.
void WriteFileToPacket(sf::Packet& pak, const File& file)
{
	pak << (uint32_t)file.Length;
	for(unsigned int i = 0; i < file.Length; ++i)
	{
		pak << (int8_t)file[i];
	}
}

#endif
