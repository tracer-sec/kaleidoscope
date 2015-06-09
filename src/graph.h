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
    std::wstring name;
    std::wstring type;
    std::wstring data;

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
    std::wstring data;
};

class Graph
{
public:
    Graph();

    void Render(QPainter &painter);
    void Iterate();

    void AddNode(Node *node);
    void AddEdge(unsigned int parentId, unsigned int childId);

    void Translate(QPointF offset);
    Node *GetNode(QPointF windowPosition, int width, int height);

protected:
    std::vector<Node *> nodes_;
    std::vector<Edge> edges_;
    double viewX_;
    double viewY_;
    double scale_;
    bool stable_;

    QPen edgePen_;
    QBrush nodeBrush_;
};

#endif // GRAPH

