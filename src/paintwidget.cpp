#include "paintwidget.h"
#include <QPainter>
#include <QMenu>
#include <QTransform>

#include <iostream>

using namespace std;

const unsigned int NODE_SIZE = 6;

PaintWidget::PaintWidget(QWidget *parent) :
    QWidget(parent),
    animating_(true),
    viewX_(0),
    viewY_(0),
    scale_(1),
    edgePen_(Qt::black),
    nodeBrush_(Qt::red)
{
    Node *n0 = new Node();
    n0->name = "london2600";
    n0->type = "person";
    n0->position.setX(20);
    n0->position.setY(20);

    graph_.AddNode(n0);

    setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(showContextMenu(const QPoint &)));
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

    QTransform transform;
    transform.translate(painter.device()->width() / 2, painter.device()->height() / 2);
    transform.translate(viewX_, viewY_);
    transform.scale(scale_, scale_);

    painter.setTransform(transform);

    painter.setPen(edgePen_);
    for (auto edge : graph_.GetEdges())
    {
        painter.drawLine(edge.parent->position, edge.child->position);
    }

    painter.setBrush(nodeBrush_);
    for (auto node : graph_.GetNodes())
    {
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
    else if (event->button() == Qt::RightButton)
    {

    }

    animating_ = false;
}

void PaintWidget::mouseReleaseEvent(QMouseEvent *event)
{
   animating_ = true;
}

void PaintWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        QPointF offset = event->pos() - startDrag_;
        viewX_ += offset.rx();
        viewY_ += offset.ry();

        startDrag_ = event->pos();
        update();
    }
}

void PaintWidget::showContextMenu(const QPoint &pos)
{
    QTransform transform;
    transform.scale(1.0 / scale_, 1.0 / scale_);
    transform.translate(-viewX_, -viewY_);
    transform.translate(-geometry().width() / 2, -geometry().height() / 2);
    QPointF worldPosition = pos * transform;

    Node *target = graph_.GetNode(worldPosition, NODE_SIZE);

    if (target != nullptr)
    {
        animating_ = false;

        QMenu contextMenu(tr("Context menu"), this);

        QAction action1("Remove node", this);
        connect(&action1, &QAction::triggered, this, [this, &target]{ removeNode(target); });
        contextMenu.addAction(&action1);
        contextMenu.addSeparator();

        // Load valid actions
        QAction action2("Some action", this);
        connect(&action2, &QAction::triggered, this, [this, &target]{ performAction(target, "test"); });
        contextMenu.addAction(&action2);

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
        // context menu of the canvas
    }
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
    cout << action << endl;
    vector<Node *> result = plugins_.RunPlugin(action, *node);

    int numNodes = result.size();
    QPointF offset(0, -30);
    QTransform transform;
    transform.rotate(360.0 / numNodes);
    for (int i = 0; i < numNodes; ++i)
    {
        Node *n = result[i];
        n->position = node->position + offset;
        graph_.AddNode(n);
        graph_.AddEdge(node->id, n->id);

        offset = offset * transform;
    }
}
