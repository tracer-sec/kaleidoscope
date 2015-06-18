#include "mainwindow.h"
#include <QApplication>

#include "py.h"

int main(int argc, char *argv[])
{
    // TODO: get this from a config file
    std::vector<std::string> paths = { "E:\\Tracer\\dev\\osint" };
    Python::InitPython(paths);

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    int result = a.exec();

    Python::ShutdownPython();

    return result;
}
