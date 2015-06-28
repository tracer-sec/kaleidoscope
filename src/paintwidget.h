#ifndef PAINTWIDGET
#define PAINTWIDGET

#include <QWidget>
#include <QMouseEvent>
#include <QPen>
#include <string>
#include "graph.h"
#include "plugins.h"

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
    bool animating_;
    Plugins plugins_;

    double viewX_;
    double viewY_;
    double scale_;

    QPen edgePen_;
    QBrush nodeBrush_;
};

#endif // PAINTWIDGET

