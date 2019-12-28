#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QProgressBar>
#include <string>
#include "graph.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void updateStatusBar(const std::string text);
    void updateNodeInfo(const Node *node);
    void updateLog(const std::string message);
    void pluginRunning();
    void pluginStopped();

private slots:
    void on_actionExit_triggered();
    void on_actionNew_triggered();

    void on_actionSave_triggered();

    void on_actionOpen_triggered();

    void on_actionSave_As_triggered();

private:
    std::string GetFilename();

    Ui::MainWindow *ui;
    std::string currentFilename_;
    bool unsavedChanges_;
    QLabel *updateLabel_;
    QProgressBar *progressBar_;
};

#endif // MAINWINDOW_H
