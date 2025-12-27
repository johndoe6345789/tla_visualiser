#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QIcon>
#include "github_importer.h"
#include "tlc_runner.h"
#include "state_graph_model.h"
#include "trace_viewer_model.h"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    
    app.setOrganizationName("TLA+ Visualiser");
    app.setOrganizationDomain("tla-visualiser.org");
    app.setApplicationName("TLA+ Visualiser");
    app.setApplicationVersion("1.0.0");

    // Register types for QML
    qmlRegisterType<tla_visualiser::StateGraphModel>("TLAVisualiser", 1, 0, "StateGraphModel");
    qmlRegisterType<tla_visualiser::TraceViewerModel>("TLAVisualiser", 1, 0, "TraceViewerModel");

    QQmlApplicationEngine engine;
    
    // Load main QML file
    const QUrl url(QStringLiteral("qml/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    
    engine.load(url);

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
