#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "newnodedialog.h"
#include "graphparser.h"

#include <QTimer>
#include <QFileDialog>

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    currentFilename_(),
    unsavedChanges_(false)
{
    ui->setupUi(this);

    setWindowTitle(QString::fromStdString("Kaleidoscope - [" + currentFilename_ + "]"));

    auto paintWidget = ui->widget;

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), paintWidget, SLOT(animate()));
    timer->start(50);

    updateLabel_ = new QLabel();
    statusBar()->addPermanentWidget(updateLabel_);
    connect(paintWidget, SIGNAL(permanentStatusEvent(QString)), updateLabel_, SLOT(setText(QString)));

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
    int result = dialog.exec();
    if (result == 1)
    {
        currentFilename_ = "";
        setWindowTitle(QString::fromStdString("Kaleidoscope - [" + currentFilename_ + "]"));
        updateNodeInfo(nullptr);
    }
}

void MainWindow::updateStatusBar(const string text)
{
    QString s = QString::fromStdString(text);
    statusBar()->showMessage(s);
}

void MainWindow::updateNodeInfo(const Node *node)
{
    if (node == nullptr)
    {
        ui->nodeNameLabel->setText("");
        ui->nodeNameLabel->setToolTip("");
        ui->nodeInfoLabel->setText("");
    }
    else
    {
        QString name = QString::fromStdString(node->type);
        name.append(" : ");
        name.append(node->name.c_str());
        QString data = QString::fromStdString(node->data);
        ui->nodeNameLabel->setText(name);
        ui->nodeNameLabel->setToolTip(name);
        ui->nodeInfoLabel->setText(data);
    }
}

void MainWindow::updateLog(const string message)
{
    QString temp = ui->logLabel->text();
    temp.append(message.c_str());
    ui->logLabel->setText(temp);
}

void MainWindow::on_actionSave_triggered()
{
    if (currentFilename_ == "")
    {
        currentFilename_ = GetFilename();
    }

    if (currentFilename_ != "")
    {
        GraphParser parser(ui->widget->getGraph());
        parser.Save(currentFilename_);
        unsavedChanges_ = false;
        updateLabel_->setText(QString::fromStdString(currentFilename_));
        setWindowTitle(QString::fromStdString("Kaleidoscope - [" + currentFilename_ + "]"));
    }
}

void MainWindow::on_actionOpen_triggered()
{
    auto filename = QFileDialog::getOpenFileName(this, "Open file", "", "Kaleidoscope graph files (*.db);; All files (* *.*)");
    if (!filename.isNull())
    {
        currentFilename_ = filename.toStdString();
        ui->widget->clearGraph();
        GraphParser parser(ui->widget->getGraph());
        parser.Load(currentFilename_);
        setWindowTitle(QString::fromStdString("Kaleidoscope - [" + currentFilename_ + "]"));
        updateLog("Graph loaded: " + currentFilename_ + "\n");
        updateNodeInfo(nullptr);
    }
}

void MainWindow::on_actionSave_As_triggered()
{
    currentFilename_ = "";
    on_actionSave_triggered();
}

string MainWindow::GetFilename()
{
    QFileDialog fileDialog(this, "Save file", "", "Kaleidoscope graph files (*.db);; All files (* *.*)");
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    fileDialog.setDefaultSuffix("db");
    if (fileDialog.exec())
        return fileDialog.selectedFiles().first().toStdString();
    else
        return "";
}
