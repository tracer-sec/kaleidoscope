#pragma once

#include <vector>
#include <string>

#include "py.h"
#include "graph.h"

class Plugins
{
public:
    Plugins();
    ~Plugins();

    std::vector<std::string> GetActions(std::string nodeType);
    PyObject *RunPlugin(std::string pluginName, Node &node);

private:
    PyObject *GetPythonNode(Node &n);

    Python::PythonModule module_;
    PyTypeObject *nodeClass_;
};

