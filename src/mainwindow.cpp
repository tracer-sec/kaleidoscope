#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTimer>
#include <QLabel>

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    auto paintWidget = findChild<PaintWidget *>("widget");

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), paintWidget, SLOT(animate()));
    timer->start(50);

    auto updateLabel = new QLabel();
    statusBar()->addPermanentWidget(updateLabel);
    connect(paintWidget, SIGNAL(permanentStatusEvent(QString)), updateLabel, SLOT(setText(QString)));

    connect(paintWidget, SIGNAL(statusEvent(const std::string)), this, SLOT(updateStatusBar(const std::string)));
    connect(paintWidget, SIGNAL(nodeSelectedEvent(const Node*)), this, SLOT(updateNodeInfo(const Node*)));
    connect(paintWidget, SIGNAL(logEvent(const std::string)), this, SLOT(updateLog(const std::string)));

    paintWidget->updateLog();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionExit_triggered()
{
    QApplication::quit();
}

void MainWindow::updateStatusBar(const string text)
{
    QString s(text.c_str());
    statusBar()->showMessage(s);
}

void MainWindow::updateNodeInfo(const Node *node)
{
    QString name(node->type.c_str());
    name.append(" : ");
    name.append(node->name.c_str());
    QString data(node->data.c_str());
    auto nodeNameLabel = findChild<QLabel *>("nodeNameLabel");
    nodeNameLabel->setText(name);
    auto nodeInfoLabel = findChild<QLabel *>("nodeInfoLabel");
    nodeInfoLabel->setText(data);
}

void MainWindow::updateLog(const string message)
{
    auto logLabel = findChild<QLabel *>("logLabel");
    logLabel->setText(message.c_str());
}
