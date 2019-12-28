#ifndef PAINTWIDGET
#define PAINTWIDGET

#include <QWidget>
#include <QMouseEvent>
#include <QPen>
#include <QFutureWatcher>
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
    void clearGraph();

    // TODO: not sure about this . . .
    Graph *getGraph() { return &graph_; }

public slots:
    void animate();

    void showContextMenu(const QPoint &pos);
    void addNode(Node *node);
    void removeNode(Node *node);
    void resumeAnimation();
    void performAction(Node *node, std::string action);
    void completeAction();
    void viewRootNode();
    void newGraph(Node *node);

signals:
    void statusEvent(const std::string message);
    void permanentStatusEvent(QString message);
    void nodeSelectedEvent(const Node *node);
    void logEvent(const std::string message);
    void pluginRunning();
    void pluginStopped();

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;

    void UpdateWorldTransform();
    void CreateBrush(const std::string str);

    Graph graph_;
    unsigned int selectedId_;
    QPointF startDrag_;
    Node *draggingNode_;
    Node *workingNode_;
    bool animating_;
    Plugins plugins_;

    double viewX_;
    double viewY_;
    double scale_;

    QTransform transform_;
    QPen edgePen_;
    std::unordered_map<std::string, QBrush> nodeBrushes_;
    QTransform arrowHeads_[2];

    QFutureWatcher<std::vector<Node *>> threadWatcher_;
};

#endif // PAINTWIDGET

