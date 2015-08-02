#include "mainwindow.h"
#include <QApplication>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDir>

#include "py.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QString appDir = a.applicationDirPath();
    QDir dir(appDir);
    QString configPath = dir.filePath("config.json");

    QFile configFile;
    configFile.setFileName(configPath);
    configFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QString json = configFile.readAll();
    configFile.close();

    QJsonDocument config = QJsonDocument::fromJson(json.toUtf8());
    auto root = config.object();
    std::string path = root["osint_path"].toString().toLocal8Bit().constData();

    std::vector<std::string> paths = { path };
    Python::InitPython(paths);

    MainWindow w;
    w.show();

    int result = a.exec();

    Python::ShutdownPython();

    return result;
}
