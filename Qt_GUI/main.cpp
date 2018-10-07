#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>

#include "mainwindow.h"

#include <QDebug>

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(icon);
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("燕山大学");
    QCoreApplication::setApplicationName("pl0_编辑器");
    QCoreApplication::setApplicationVersion(QT_VERSION_STR);

    QCommandLineParser parser;
    parser.setApplicationDescription(QCoreApplication::applicationName());
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("file", "the file to open");
    parser.process(app);

    qDebug() << "test";
    MainWindow w;
    if (not parser.positionalArguments().isEmpty())
    {
        w.loadFile(parser.positionalArguments().first());
    }
    w.show();

    return app.exec();
}
