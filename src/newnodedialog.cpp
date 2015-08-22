#include "newnodedialog.h"
#include "ui_newnodedialog.h"
#include <QMessageBox>

using namespace std;

NewNodeDialog::NewNodeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewNodeDialog)
{
    ui->setupUi(this);
}

NewNodeDialog::~NewNodeDialog()
{
    delete ui;
}

void NewNodeDialog::accept()
{
    auto type = ui->nodeTypeEdit->text();
    auto name = ui->nodeNameEdit->text();

    if (type.count() == 0 || name.count() == 0)
    {
        QMessageBox::information(this, "Woah there cowboy", "Please make sure you have entered a node type and name");
    }
    else
    {
        Node *n = new Node();
        n->type = string(type.toStdString());
        n->name = string(name.toStdString());
        emit nodeCreatedEvent(n);

        QDialog::accept();
    }
}
