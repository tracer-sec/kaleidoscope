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
}

Plugins::~Plugins()
{
    Py_DECREF(nodeClass_);
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
    PyObject *args = Py_BuildValue("(s)", pluginName.c_str());
    PyObject *plugin = module_.CallFunction("fetch", args);
    Py_DECREF(args);

    vector<Node *> result;

    if (plugin)
    {
        auto func = PyDict_GetItemString(plugin, "func");

        PyErr_Print();

        if (func)
        {
            PyObject *pyNode = GetPythonNode(node);

            args = Py_BuildValue("(O)", pyNode);
            PyObject *data = PyObject_CallObject(func, args);
            Py_DECREF(args);

            size_t length = PyList_Size(data);
            for (unsigned int i = 0; i < length; ++i)
                result.push_back(GetNode(PyList_GetItem(data, i)));

            Py_DECREF(data);
            Py_DECREF(pyNode);
        }

        Py_DECREF(plugin);
    }

    return result;
}

PyObject *Plugins::GetPythonNode(Node &n)
{
    // TODO: fix data creation (import via JSON)

    PyObject *newNodeArgs = Py_BuildValue("ss", n.type.c_str(), n.name.c_str());
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

