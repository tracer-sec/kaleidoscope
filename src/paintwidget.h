#ifndef PAINTWIDGET
#define PAINTWIDGET

#include <QWidget>
#include <QMouseEvent>
#include <string>
#include "graph.h"

class PaintWidget : public QWidget
{
    Q_OBJECT

public:
    PaintWidget(QWidget *parent);

public slots:
    void animate();

    void showContextMenu(const QPoint &pos);
    void removeNode(Node *node);
    void resumeAnimation();
    void performAction(Node *node, std::string action);

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

    Graph graph_;
    QPointF startDrag_;
    unsigned int nextId_;
    bool animating_;
};

#endif // PAINTWIDGET

