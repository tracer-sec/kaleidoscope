#include "paintwidget.h"
#include <QPainter>
#include <QMenu>
#include <QTransform>

#include <iostream>

using namespace std;

PaintWidget::PaintWidget(QWidget *parent) :
    QWidget(parent),
    animating_(true)
{
    int i = 0;

    Node *n0 = new Node();
    n0->id = 1;
    n0->name = "test";
    n0->type = "person";
    n0->position.setX(20);
    n0->position.setY(20);

    Node *n1 = new Node();
    n1->id = 2;
    n1->name = "test";
    n1->type = "twitter";
    n1->position.setX(-20);
    n1->position.setY(0);

    Node *n2 = new Node();
    n2->id = 3;
    n2->name = "test@test.com";
    n2->type = "email";
    n2->position.setX(30);
    n2->position.setY(10);

    graph_.AddNode(n0);
    graph_.AddNode(n1);
    graph_.AddNode(n2);
    graph_.AddEdge(1, 2);
    graph_.AddEdge(1, 3);
    //graph_.AddEdge(2, 3);

    nextId_ = 4;

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

    graph_.Render(painter);

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
        graph_.Translate(offset);
        startDrag_ = event->pos();
        update();
    }
}

void PaintWidget::showContextMenu(const QPoint &pos)
{
    Node *target = graph_.GetNode(pos, geometry().width(), geometry().height());

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
    if (action == "test")
    {
        int numNodes = static_cast<int>((static_cast<float>(qrand()) / RAND_MAX) * 5 + 1);
        QPointF offset(0, -10);
        QTransform transform;
        transform.rotate(360.0 / numNodes);
        for (int i = 0; i < numNodes; ++i)
        {
            Node *n = new Node();
            n->id = nextId_;
            n->position = node->position + offset;
            graph_.AddNode(n);
            graph_.AddEdge(node->id, n->id);

            nextId_ ++;
            offset = offset * transform;
        }
    }
    else
    {
        int i = 1;
        cout << action << endl;
    }
}
