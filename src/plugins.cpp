#include "plugins.h"

using namespace std;
using namespace Python;

Plugins::Plugins() :
    module_("plugins")
{
    Python::GetError();
    PyObject *args = Py_BuildValue("(s)", "config.json");
    module_.CallFunction("load_all", args);
    Py_DECREF(args);

    Python::GetError();
    PythonModule model("model");
    Python::GetError();
    nodeClass_ = model.GetClass("Node");

    catcher_ = module_.GetAttribute("log_dump");
    func_ = module_.GetAttribute("run");
}

Plugins::~Plugins()
{
    // Crash bang and wallop if this is here. Not sure why? :-/
    //Py_DECREF(catcher_);
    Py_DECREF(nodeClass_);
    Py_DECREF(func_);
}

vector<string> Plugins::GetActions(string nodeType)
{
    PyObject *args = Py_BuildValue("(s)", nodeType.c_str());
    PyObject *plugins = module_.CallFunction("fetch_actions", args);
    Py_DECREF(args);

    vector<string> result;

    if (plugins)
    {
        size_t length = PyList_Size(plugins);
        for (unsigned int i = 0; i < length; ++i)
        {
            auto plugin = PyList_GetItem(plugins, i);
            auto func = PyDict_GetItemString(plugin, "func");
            string funcName = GetStringAttribute(func, "__name__");

            result.push_back(funcName);
        }

        Py_DECREF(plugins);
    }

    return result;
}

vector<Node *> Plugins::RunPlugin(string pluginName, Node &node)
{
    vector<Node *> result;

    PyObject *pyNode = GetPythonNode(node);

    PyObject *args = Py_BuildValue("Os", pyNode, pluginName.c_str());
    PyObject *data = PyObject_CallObject(func_, args);
    Py_DECREF(args);

    size_t length = PyList_Size(data);
    for (unsigned int i = 0; i < length; ++i)
        result.push_back(GetNode(PyList_GetItem(data, i)));

    UpdateNode(pyNode, node);

    Py_DECREF(data);
    Py_DECREF(pyNode);

    return result;
}

PyObject *Plugins::GetPythonNode(Node &n)
{
    PyObject *newNodeArgs = Py_BuildValue("sssI", n.type.c_str(), n.name.c_str(), n.data.c_str(), n.id);
    PyObject *newNode = PyObject_CallObject(reinterpret_cast<PyObject *>(nodeClass_), newNodeArgs);
    Py_DECREF(newNodeArgs);
    return newNode;
}

Node *Plugins::GetNode(PyObject *n)
{
    Node *node = new Node();
    node->name = Python::GetStringAttribute(n, "name");
    node->type = Python::GetStringAttribute(n, "node_type");
    node->data = Python::GetStringAttribute(n, "data_json");
    return node;
}

void Plugins::UpdateNode(PyObject *p, Node &n)
{
    n.data = Python::GetStringAttribute(p, "data_json");
}

string Plugins::GetLog()
{
    PyObject *output = PyObject_GetAttrString(catcher_, "value");
    string log(PyString_AsString(output));
    Py_DECREF(output);
    return log;
}

