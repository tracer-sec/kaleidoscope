#ifndef GRAPH
#define GRAPH

#include <QPointF>
#include <QPen>
#include <QBrush>
#include <QPainter>
#include <string>
#include <vector>

struct Node
{
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

    void Render(QPainter &painter);
    void Iterate();

    void AddNode(Node *node);
    void AddEdge(unsigned int parentId, unsigned int childId);

    void RemoveNode(Node *node);

    Node *GetNode(QPointF worldPosition);

protected:
    std::vector<Node *> nodes_;
    std::vector<Edge> edges_;
    bool stable_;

    QPen edgePen_;
    QBrush nodeBrush_;
    unsigned int nextId_;
};

#endif // GRAPH

