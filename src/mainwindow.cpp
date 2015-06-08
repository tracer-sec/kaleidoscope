#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTimer>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), findChild<QWidget *>("widget"), SLOT(animate()));
    timer->start(50);
}

MainWindow::~MainWindow()
{
    delete ui;
}
