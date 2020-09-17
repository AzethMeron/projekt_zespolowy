#ifndef PYTHON_HPP
#define PYTHON_HPP

//#include <boost/python.hpp>

#include <stdio.h>
#include <vector>
#include <string>
#include <pthread.h>

pthread_mutex_t python_mutex;

class CPy
{
	public:
		CPy()
		{
			pthread_mutex_lock(&python_mutex);
			Py_Initialize();
		}
		~CPy()
		{
			Py_Finalize();
			pthread_mutex_unlock(&python_mutex);
		}
};

void CallScript(string Name, vector<string> Args)
{	
	CPy temp;
	// Header
	string caller = "import sys \nsys.argv=['"+Name+"'";
	for(unsigned int i = 0; i < Args.size(); ++i)	
	{
		caller = caller + ','+"'"+Args[i]+"'";
	}
	caller += "]\n";
	
	// Caller
	caller += "with open ('"+Name+"') as f:\n \
	code = compile (f.read(), '"+Name+"', 'exec')\n \
	exec(code)";
	
	PyRun_SimpleString(caller.c_str());
	//PyRun_SimpleString("sys.modules[__name__].__dict__.clear()");
}

#endif
