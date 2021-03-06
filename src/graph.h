#ifndef GRAPH
#define GRAPH

#include <QPointF>
#include <string>
#include <vector>

struct Node
{
    Node() :
        id(0),
        data("{}")
    {

    }

    unsigned int id;
    std::string name;
    std::string type;
    std::string data;

    QPointF position;
    QPointF force;
};

struct Edge
{
    Edge(Node *p, Node *c) :
        parent(p),
        child(c)
    { }

    Node *parent;
    Node *child;
    std::string data;
};

class Graph
{
public:
    Graph();

    void Iterate(unsigned int lockedNodeId);

    Node *AddNode(Node *node);
    void AddEdge(unsigned int parentId, unsigned int childId);

    void RemoveNode(Node *node);

    Node *GetNode(QPointF worldPosition, unsigned int nodeSize);
    Node *GetNode(std::string type, std::string name);

    std::vector<Node *> GetNodes() { return nodes_; }
    std::vector<Edge> GetEdges() { return edges_; }

    std::vector<Node *> GetAttached(const Node *node);

protected:
    std::vector<Node *> nodes_;
    std::vector<Edge> edges_;
    bool stable_;

    unsigned int nextId_;
};

#endif // GRAPH

