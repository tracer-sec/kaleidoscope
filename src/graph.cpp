#include "graph.h"

const double CHARGE = 1000;
const double SPRING = 0.05;
const double EQUILIBRIUM = 40.0;

Graph::Graph() :
    viewX_(0),
    viewY_(0),
    scale_(1),
    stable_(true),
    edgePen_(Qt::black),
    nodeBrush_(Qt::red)
{
    
}

void Graph::Render(QPainter &painter)
{
    QTransform transform;
    transform.translate(painter.device()->width() / 2, painter.device()->height() / 2);
    transform.translate(viewX_, viewY_);
    transform.scale(scale_, scale_);
    
    painter.setTransform(transform);
    
    painter.setPen(edgePen_);
    for (auto edge : edges_)
    {
        painter.drawLine(edge.parent->position, edge.child->position);
    }

    painter.setBrush(nodeBrush_);
    for (auto node : nodes_)
    {
        painter.drawEllipse(node->position, 6, 6);
    }
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

void Graph::AddNode(Node *node)
{
    // TODO: check for duplicates
    nodes_.push_back(node);
    stable_ = false;
}

void Graph::AddEdge(unsigned int parentId, unsigned int childId)
{
    Node *parent = *find_if(nodes_.begin(), nodes_.end(), [&](const Node *a) { return a->id == parentId; });
    Node *child = *find_if(nodes_.begin(), nodes_.end(), [&](const Node *a) { return a->id == childId; });
    Edge edge(parent, child);
    edges_.push_back(edge);
    stable_ = false;
}

void Graph::Translate(QPointF offset)
{
    viewX_ += offset.rx();
    viewY_ += offset.ry();
}

Node *Graph::GetNode(QPointF windowPosition, int width, int height)
{
    QTransform transform;
    transform.scale(1.0 / scale_, 1.0 / scale_);
    transform.translate(-viewX_, -viewY_);
    transform.translate(-width / 2, -height / 2);

    QPointF worldPosition = windowPosition * transform;

    Node *result = nullptr;
    for (auto n : nodes_)
    {
        QPointF v = n->position - worldPosition;
        double d = v.rx() * v.rx() + v.ry() * v.ry();
        if (d < 36)
        {
            result = n;
            break;
        }
    }

    return result;
}

