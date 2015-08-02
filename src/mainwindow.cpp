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

    auto paintWidget = findChild<QWidget *>("widget");

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), paintWidget, SLOT(animate()));
    timer->start(50);

    auto updateLabel = new QLabel("Foo");
    statusBar()->addPermanentWidget(updateLabel);
    connect(paintWidget, SIGNAL(permanentStatusEvent(QString)), updateLabel, SLOT(setText(QString)));

    connect(paintWidget, SIGNAL(statusEvent(const std::string)), this, SLOT(updateStatusBar(const std::string)));
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
