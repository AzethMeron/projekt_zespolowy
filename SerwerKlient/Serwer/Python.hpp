#ifndef PYTHON_HPP
#define PYTHON_HPP

#include "/usr/include/python2.7/Python.h"
#include <stdio.h>
#include <vector>
#include <string>

// Na podstawie: https://docs.python.org/2/extending/embedding.html

void CallScript(string Name, string FuncName, vector<string> Args)
{
	PyObject *pName, *pModule, *pFunc;
    PyObject *pArgs, *pValue;
    int i;

    pName = PyString_FromString(Name.c_str());
    pModule = PyImport_Import(pName);
    Py_DECREF(pName);

    if (pModule != NULL) {
        pFunc = PyObject_GetAttrString(pModule, FuncName.c_str() );
        /* pFunc is a new reference */

        if (pFunc && PyCallable_Check(pFunc)) {
            pArgs = PyTuple_New(Args.size());
            for (i = 0; i < (int)Args.size(); ++i) {
                pValue = PyString_FromString((Args[i]).c_str());
                if (!pValue) {
                    Py_DECREF(pArgs);
                    Py_DECREF(pModule);
                    fprintf(stderr, "Cannot convert argument\n");
                    return;
                }
                /* pValue reference stolen here: */
                PyTuple_SetItem(pArgs, i, pValue);
            }
            pValue = PyObject_CallObject(pFunc, pArgs);
            Py_DECREF(pArgs);
            if (pValue != NULL) {
                printf("Result of call: %ld\n", PyInt_AsLong(pValue));
                Py_DECREF(pValue);
            }
            else {
                Py_DECREF(pFunc);
                Py_DECREF(pModule);
                PyErr_Print();
                fprintf(stderr,"Call failed\n");
                return;
            }
        }
        else {
            if (PyErr_Occurred())
                PyErr_Print();
            fprintf(stderr, "Cannot find function \"%s\"\n", FuncName.c_str());
        }
        Py_XDECREF(pFunc);
        Py_DECREF(pModule);
    }
    else {
        PyErr_Print();
        fprintf(stderr, "Failed to load \"%s\"\n", Name.c_str());
        return;
    }
    return;
}

#endif
