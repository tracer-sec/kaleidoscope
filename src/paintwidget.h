#ifndef PAINTWIDGET
#define PAINTWIDGET

#include <QWidget>
#include <QMouseEvent>
#include "graph.h"

class PaintWidget : public QWidget
{
    Q_OBJECT

public:
    PaintWidget(QWidget *parent);

public slots:
    void animate();

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

    Graph graph_;
    QPointF startDrag_;
    unsigned int nextId_;
};

#endif // PAINTWIDGET

