#include "graph.h"
#include <algorithm>

const double CHARGE = 750;
const double SPRING = 0.1;
const double EQUILIBRIUM = 60.0;

using namespace std;

Graph::Graph() :
    stable_(true),
    nextId_(1)
{
    
}

void Graph::Iterate()
{
    if (stable_)
        return;
    
    // TODO: Something odd happening here . . .
    for (unsigned int i = 0; i < nodes_.size(); ++i)
    {
        Node *n0 = nodes_[i];
        for (unsigned int j = i + 1; j < nodes_.size(); ++j)
        {
            Node *n1 = nodes_[j];
            QPointF v = n0->position - n1->position;
            double d = sqrt(v.rx() * v.rx() + v.ry() * v.ry());
            d = std::max(0.1, d);
            v /= d;
            
            double push = CHARGE * 1.f / (d * d);
            n0->force += v * push;
            n1->force -= v * push;
        }
    }
    
    for (auto e : edges_)
    {
        QPointF v = e.child->position - e.parent->position;
        double d = sqrt(v.rx() * v.rx() + v.ry() * v.ry());
        v /= d;

        double push = SPRING * (d - EQUILIBRIUM);
        e.parent->force += v * push;
        e.child->force -= v * push;
    }
    
    double tickMovement = 0;
    for (auto n : nodes_)
    {
        tickMovement += sqrt(n->force.rx() * n->force.rx() + n->force.ry() * n->force.ry());
        n->position += n->force;
        n->force = QPointF();
    }
    
    stable_ = tickMovement < 0.0001;
}

Node *Graph::AddNode(Node *node)
{
    Node *n = GetNode(node->type, node->name);
    if (n == nullptr)
    {
        node->id = nextId_;
        nodes_.push_back(node);
        nextId_ ++;
        n = node;
    }
    stable_ = false;
    return n;
}

void Graph::AddEdge(unsigned int parentId, unsigned int childId)
{
    Node *parent = *find_if(nodes_.begin(), nodes_.end(), [&](const Node *a) { return a->id == parentId; });
    Node *child = *find_if(nodes_.begin(), nodes_.end(), [&](const Node *a) { return a->id == childId; });
    Edge edge(parent, child);
    edges_.push_back(edge);
    stable_ = false;
}

void Graph::RemoveNode(Node *node)
{
    nodes_.erase(std::remove(nodes_.begin(), nodes_.end(), node), nodes_.end());
    edges_.erase(remove_if(edges_.begin(), edges_.end(), [&](Edge e) { return e.parent == node || e.child == node; }), edges_.end());
    delete node;
}

Node *Graph::GetNode(QPointF worldPosition, unsigned int nodeSize)
{
    Node *result = nullptr;
    for (auto n : nodes_)
    {
        QPointF v = n->position - worldPosition;
        double d = v.rx() * v.rx() + v.ry() * v.ry();
        if (d < nodeSize * nodeSize)
        {
            result = n;
            break;
        }
    }

    return result;
}

Node *Graph::GetNode(string type, string name)
{
    auto n = find_if(nodes_.begin(), nodes_.end(), [&](const Node *a) { return a->type == type && a->name == name; });
    return n == nodes_.end() ? nullptr : *n;
}

