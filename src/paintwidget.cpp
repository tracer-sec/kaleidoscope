#include "paintwidget.h"
#include <QPainter>
#include <QMenu>
#include <QTransform>

#include <iostream>
#include <sstream>

using namespace std;

const unsigned int NODE_SIZE = 6;

PaintWidget::PaintWidget(QWidget *parent) :
    QWidget(parent),
    animating_(true),
    viewX_(0),
    viewY_(0),
    scale_(1),
    edgePen_(Qt::black),
    selectedId_(0)
{
    setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(showContextMenu(const QPoint &)));

    nodeBrushes_ = std::unordered_map<string, QBrush>({
        { "DEFAULT", QBrush(Qt::gray) },
        { "SELECTED", QBrush(Qt::white) },
        { "person", QBrush(Qt::red) },
        { "twitter", QBrush(Qt::blue) },
        { "reddit", QBrush(Qt::green) },
        { "domain", QBrush(Qt::lightGray) },
        { "email", QBrush(Qt::yellow) }
    });

    setMouseTracking(true);

    arrowHeads_[0].rotate(30);
    arrowHeads_[1].rotate(-30);
}

void PaintWidget::updateLog()
{
    logEvent(plugins_.GetLog());
}

void PaintWidget::newGraph()
{
    graph_ = Graph();
}

void PaintWidget::animate()
{
    if (animating_)
    {
        graph_.Iterate();
        update();
    }
}

void PaintWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);

    UpdateWorldTransform();
    painter.setTransform(transform_);

    painter.setPen(edgePen_);
    for (auto edge : graph_.GetEdges())
    {
        painter.drawLine(edge.parent->position, edge.child->position);
        QPointF v = edge.child->position - edge.parent->position;
        double d = sqrt(v.rx() * v.rx() + v.ry() * v.ry());
        v = v / d;
        QPointF nose = edge.child->position - v * NODE_SIZE;
        v = v * 6;
        painter.drawLine(nose, nose - (v * arrowHeads_[0]));
        painter.drawLine(nose, nose - (v * arrowHeads_[1]));
    }

    for (auto node : graph_.GetNodes())
    {
        if (node->id == selectedId_)
            painter.setBrush(nodeBrushes_["SELECTED"]);
        else
        {
            auto brush = nodeBrushes_.find(node->type);
            if (brush == nodeBrushes_.end())
                painter.setBrush(nodeBrushes_["DEFAULT"]);
            else
                painter.setBrush((*brush).second);
        }
        painter.drawEllipse(node->position, NODE_SIZE, NODE_SIZE);
    }

    painter.end();
}

void PaintWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        startDrag_ = event->pos();
    }

    animating_ = false;
}

void PaintWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        UpdateWorldTransform();
        QPointF worldPosition = event->pos() * transform_.inverted();
        Node *target = graph_.GetNode(worldPosition, NODE_SIZE);
        if (target != nullptr)
        {
            selectedId_ = target->id;
            nodeSelectedEvent(target);
        }
    }

    animating_ = true;
}

void PaintWidget::mouseMoveEvent(QMouseEvent *event)
{
    UpdateWorldTransform();
    QPointF worldPosition = event->pos() * transform_.inverted();

    Node *target = graph_.GetNode(worldPosition, NODE_SIZE);
    ostringstream s;
    if (target != nullptr)
    {
        s << target->type << " : " << target->name;
    }
    statusEvent(s.str());

    if (event->buttons() & Qt::LeftButton)
    {
        QPointF offset = event->pos() - startDrag_;
        viewX_ += offset.rx() / scale_;
        viewY_ += offset.ry() / scale_;

        startDrag_ = event->pos();
        update();
    }
}

void PaintWidget::showContextMenu(const QPoint &pos)
{
    UpdateWorldTransform();
    QPointF worldPosition = pos * transform_.inverted();

    animating_ = false;

    Node *target = graph_.GetNode(worldPosition, NODE_SIZE);

    if (target != nullptr)
    {
        QMenu contextMenu(tr("Context menu"), this);

        QAction action1("Remove node", this);
        connect(&action1, &QAction::triggered, this, [this, &target]{ removeNode(target); });
        contextMenu.addAction(&action1);
        contextMenu.addSeparator();

        // Load valid actions
        auto actionNames = plugins_.GetActions(target->type);
        for (string actionName : actionNames)
        {
            QAction *action = new QAction(actionName.c_str(), this);
            connect(action, &QAction::triggered, this, [=, &target]{ performAction(target, actionName); });
            contextMenu.addAction(action);
        }

        contextMenu.exec(mapToGlobal(pos));

        connect(&contextMenu, SIGNAL(destroyed()), this, SLOT(resumeAnimation()));
    }
    else
    {
        QMenu contextMenu(tr("Context menu"), this);

        connect(&contextMenu, SIGNAL(destroyed()), this, SLOT(resumeAnimation()));

        QAction action1("Go to root node", this);
        connect(&action1, &QAction::triggered, this, &PaintWidget::viewRootNode);
        contextMenu.addAction(&action1);

        contextMenu.exec(mapToGlobal(pos));

        connect(&contextMenu, SIGNAL(destroyed()), this, SLOT(resumeAnimation()));
    }
}

void PaintWidget::addNode(Node *node)
{
    graph_.AddNode(node);
}

void PaintWidget::removeNode(Node *target)
{
    graph_.RemoveNode(target);
}

void PaintWidget::resumeAnimation()
{
    animating_ = true;
}

void PaintWidget::performAction(Node *node, string action)
{
    logEvent(action);
    vector<Node *> result = plugins_.RunPlugin(action, *node);

    // Find the direction furthest away from
    vector<Node *> attached = graph_.GetAttached(node);
    QPointF v(0, -1);
    for (auto n : attached)
    {
        v += node->position - n->position;
    }
    double d = sqrt(v.rx() * v.rx() + v.ry() * v.ry());
    d = max(1.0, d);
    v /= d;
    // Unit length away from all the attached nodes

    int numNodes = result.size();
    QPointF offset = v * 30;
    double angle = 0;
    double angleStep = 360.0 / numNodes;
    for (int i = 0; i < numNodes; ++i)
    {
        angle += angleStep * i * (i % 2 == 0 ? -1 : 1);
        QTransform transform;
        transform.rotate(angle);

        Node *n = result[i];
        n->position = node->position + offset * transform;
        n = graph_.AddNode(n);
        graph_.AddEdge(node->id, n->id);
    }

    ostringstream s;
    s << numNodes << " new nodes added";
    permanentStatusEvent(QString::fromUtf8(s.str().c_str()));
    logEvent(plugins_.GetLog());

    // Reselect the node, so the UI updates the node info UI
    if (selectedId_ == node->id)
        nodeSelectedEvent(node);
}

void PaintWidget::viewRootNode()
{
    scale_ = 1;
    Node *root = graph_.GetNodes()[0];
    viewX_ = root->position.x() * -1;
    viewY_ = root->position.y() * -1;
}

void PaintWidget::wheelEvent(QWheelEvent *event)
{
    double d = static_cast<double>(event->delta()) / 360 + 1;
    scale_ *= d;
}

void PaintWidget::UpdateWorldTransform()
{
    transform_.reset();
    transform_.translate(geometry().width() / 2, geometry().height() / 2);
    transform_.scale(scale_, scale_);
    transform_.translate(viewX_, viewY_);
}
