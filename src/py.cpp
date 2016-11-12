#include "py.h"

using namespace std;
using namespace Python;

void Python::InitPython(vector<string> paths)
{
    Py_SetProgramName("python_embed");
    Py_Initialize();
    PyEval_InitThreads();

    PyObject* sysPath = PySys_GetObject("path");
    for (auto newPath : paths)
    {
        PyObject* p = PyString_FromString(newPath.c_str());
        PyList_Append(sysPath, p);
        Py_DECREF(p);
    }

    // https://stackoverflow.com/questions/4307187/how-to-catch-python-stdout-in-c-code
    std::string stdOutErr =
    "import sys\n\
class CatchOutErr:\n\
    def __init__(self):\n\
        self._value = ''\n\
    def write(self, txt):\n\
        self._value += txt\n\
    @property\n\
    def value(self):\n\
        v = self._value\n\
        self._value = ''\n\
        return v\n\
catchOutErr = CatchOutErr()\n\
sys.stdout = catchOutErr\n\
sys.stderr = catchOutErr\n\
import plugins\n\
plugins.log_dump = catchOutErr\n\
"; //this is python code to redirect stdouts/stderr

    PyRun_SimpleString(stdOutErr.c_str());

    GetError();
}

void Python::ShutdownPython()
{
    Py_Finalize();
}

string Python::GetStringAttribute(PyObject *o, string name)
{
    PyObject *foo = PyObject_GetAttrString(o, name.c_str());
    string result(PyString_AsString(foo));
    Py_DECREF(foo);
    return result;
}

vector<PyObject *> Python::GetListAttribute(PyObject *o, string name)
{
    PyObject *foo = PyObject_GetAttrString(o, name.c_str());
    size_t length = PyList_Size(foo);
    vector<PyObject *> result(length);
    for (unsigned int i = 0; i < length; ++i)
        result[i] = PyList_GetItem(foo, i);
    Py_DECREF(foo);
    return result;
}

long Python::GetIntAttribute(PyObject *o, std::string name)
{
    PyObject *foo = PyObject_GetAttrString(o, name.c_str());
    long result = PyInt_AsLong(foo);
    Py_DECREF(foo);
    return result;
}

PythonModule::PythonModule(string moduleName)
{
    PyObject *name = PyString_FromString(moduleName.c_str());
    handle_ = PyImport_Import(name);
    Py_DECREF(name);
}

PythonModule::~PythonModule()
{
    Py_DECREF(handle_);
}

PyTypeObject *PythonModule::GetClass(string className)
{
    PyObject *name = PyString_FromString(className.c_str());
    PyTypeObject *result = reinterpret_cast<PyTypeObject *>(PyObject_GetAttr(handle_, name));
    Py_DECREF(name);
    return result;
}

PyObject *PythonModule::GetAttribute(string attributeName)
{
    return PyObject_GetAttrString(handle_, attributeName.c_str());
}

PyObject *PythonModule::CallFunction(string functionName)
{
    PyObject *result = nullptr;
    PyObject *func = PyObject_GetAttrString(handle_, functionName.c_str());
    if (func != nullptr && PyCallable_Check(func))
        result = PyObject_CallObject(func, nullptr);
    Py_DECREF(func);
    return result;
}

PyObject *PythonModule::CallFunction(string functionName, PyObject *args)
{
    PyObject *result = nullptr;
    PyObject *func = PyObject_GetAttrString(handle_, functionName.c_str());
    if (func != nullptr && PyCallable_Check(func))
        result = PyObject_CallObject(func, args);
    Py_DECREF(func);
    return result;
}

void Python::GetError()
{
    PyObject *type, *value, *traceback;
    PyErr_Fetch(&type, &value, &traceback);
    PyErr_NormalizeException(&type, &value, &traceback);
    if (type)
    {
        string t(PyString_AsString(PyObject_Str(type)));
        string v(PyString_AsString(PyObject_Str(value)));
        string s(PyString_AsString(PyObject_Str(traceback)));
    }
}



