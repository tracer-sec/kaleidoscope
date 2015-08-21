#ifndef PAINTWIDGET
#define PAINTWIDGET

#include <QWidget>
#include <QMouseEvent>
#include <QPen>
#include <string>
#include <unordered_map>
#include "graph.h"
#include "plugins.h"

class PaintWidget : public QWidget
{
    Q_OBJECT

public:
    PaintWidget(QWidget *parent);

    void updateLog();
    void newGraph();

public slots:
    void animate();

    void showContextMenu(const QPoint &pos);
    void addNode(Node *node);
    void removeNode(Node *node);
    void resumeAnimation();
    void performAction(Node *node, std::string action);
    void viewRootNode();

signals:
    void statusEvent(const std::string message);
    void permanentStatusEvent(QString message);
    void nodeSelectedEvent(const Node *node);
    void logEvent(const std::string message);

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;

    void UpdateWorldTransform();

    Graph graph_;
    unsigned int selectedId_;
    QPointF startDrag_;
    bool animating_;
    Plugins plugins_;

    double viewX_;
    double viewY_;
    double scale_;

    QTransform transform_;
    QPen edgePen_;
    std::unordered_map<std::string, QBrush> nodeBrushes_;
};

#endif // PAINTWIDGET

