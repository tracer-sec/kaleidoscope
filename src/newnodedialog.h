#ifndef NEWNODEDIALOG_H
#define NEWNODEDIALOG_H

#include <QDialog>
#include "graph.h"

namespace Ui {
class NewNodeDialog;
}

class NewNodeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewNodeDialog(QWidget *parent = 0);
    ~NewNodeDialog();

public slots:
    void accept();

signals:
    void nodeCreatedEvent(Node *node);

private:
    Ui::NewNodeDialog *ui;
};

#endif // NEWNODEDIALOG_H
