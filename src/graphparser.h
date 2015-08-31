#ifndef GRAPHPARSER
#define GRAPHPARSER

#include <string>
#include "graph.h"

class GraphParser
{
public:
    GraphParser(Graph *graph);
    int Load(std::string filename);
    int Save(std::string filename);

protected:
    Graph *graph_;
};

#endif // GRAPHPARSER

