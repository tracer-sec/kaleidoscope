#include "paintwidget.h"
#include <QPainter>
#include <QMenu>

PaintWidget::PaintWidget(QWidget *parent) :
    QWidget(parent),
    animating_(true)
{
    int i = 0;

    Node *n0 = new Node();
    n0->id = 1;
    n0->position.setX(20);
    n0->position.setY(20);

    Node *n1 = new Node();
    n1->id = 2;
    n1->position.setX(-20);
    n1->position.setY(0);

    Node *n2 = new Node();
    n2->id = 3;
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
        Node *parent = graph_.GetNode(event->pos(), geometry().width(), geometry().height());
        if (parent != nullptr)
        {
            Node *n = new Node();
            n->id = nextId_;
            n->position = parent->position + QPointF(10, 10);
            graph_.AddNode(n);
            graph_.AddEdge(parent->id, n->id);
            nextId_ ++;
        }
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
