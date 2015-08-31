#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "newnodedialog.h"
#include "graphparser.h"

#include <QTimer>
#include <QLabel>
#include <QFileDialog>

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    auto paintWidget = ui->widget;

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

void MainWindow::on_actionNew_triggered()
{
    auto paintWidget = ui->widget;
    NewNodeDialog dialog;
    connect(&dialog, &NewNodeDialog::nodeCreatedEvent, paintWidget, &PaintWidget::newGraph);
    dialog.exec();
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
    ui->nodeNameLabel->setText(name);
    ui->nodeInfoLabel->setText(data);
}

void MainWindow::updateLog(const string message)
{
    QString temp = ui->logLabel->text();
    temp.append(message.c_str());
    ui->logLabel->setText(temp);
}

void MainWindow::on_actionSave_triggered()
{
    auto filename = QFileDialog::getSaveFileName(this, "Save file", "", "Kaleidoscope graph files (*.db);; All files (* *.*)");
    if (!filename.isNull())
    {
        GraphParser parser(ui->widget->getGraph());
        parser.Save(filename.toStdString());
    }
}

void MainWindow::on_actionOpen_triggered()
{
    auto filename = QFileDialog::getOpenFileName(this, "Open file", "", "Kaleidoscope graph files (*.db);; All files (* *.*)");
    if (!filename.isNull())
    {
        ui->widget->clearGraph();
        GraphParser parser(ui->widget->getGraph());
        parser.Load(filename.toStdString());
    }
}
