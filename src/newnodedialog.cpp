#include "newnodedialog.h"
#include "ui_newnodedialog.h"

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
    auto type = findChild<QLineEdit *>("nodeTypeEdit");
    auto name = findChild<QLineEdit *>("nodeNameEdit");

    Node *n = new Node();
    n->type = string(type->text().toStdString());
    n->name = string(name->text().toStdString());
    emit nodeCreatedEvent(n);

    QDialog::accept();
}
