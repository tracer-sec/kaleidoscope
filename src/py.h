#pragma once

#include <string>
#include <vector>

#undef _DEBUG // Because we're missing the Python debug lib
#include "Python.h"

namespace Python
{
    void InitPython(std::vector<std::string> paths);
    void ShutdownPython();

    std::string GetStringAttribute(PyObject *o, std::string name);
    std::vector<PyObject *> GetListAttribute(PyObject *o, std::string name);
    long GetIntAttribute(PyObject *o, std::string name);

    class PythonModule
    {
    public:
        PythonModule(std::string moduleName);
        ~PythonModule();

        PyTypeObject *GetClass(std::string className);
        PyObject *GetAttribute(std::string attributeName);
        PyObject *CallFunction(std::string functionName);
        PyObject *CallFunction(std::string functionName, PyObject *args);

    protected:
        PyObject *handle_;
    };

    void GetError();
}

