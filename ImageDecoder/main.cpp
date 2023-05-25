#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <qdir.h>
#include "TableModel.h"

QStringList supportedTypes({ QString("*.bmp"), QString("*.BMP"), QString("*.barch"), QString("*.BARCH") });

int main(int argc, char *argv[])
{
#if defined(Q_OS_WIN)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    TableModel model;
    auto path = argc > 1 && QDir(argv[1]).exists() ? argv[1] : QCoreApplication::applicationDirPath();
    QDir directory(path);
    QStringList images = directory.entryList(supportedTypes, QDir::Files);
    model.init(path, images);
    engine.rootContext()->setContextProperty("table_model", &model);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
