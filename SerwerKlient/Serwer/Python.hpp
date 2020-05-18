#ifndef PYTHON_HPP
#define PYTHON_HPP

// Use 'locate Python.h' to get path
#include "/usr/include/python3.6m/Python.h"

#include <stdio.h>
#include <vector>
#include <string>

class CPy
{
	public:
		CPy()
		{
			Py_Initialize();
		}
		~CPy()
		{
			Py_Finalize();
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
