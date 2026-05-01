#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtQml/qqml.h>
#include <QQuickStyle>
#ifdef HAS_QTWEBVIEW
#include <QtWebView/QtWebView>
#endif

#include "appcontroller.h"
#include "parkinglotmodel.h"
#include "settingsmanager.h"
#include "simulationmanager.h"

int main(int argc, char *argv[])
{
#ifdef HAS_QTWEBVIEW
    // Must be called before QGuiApplication on Android (wraps native WebView).
    QtWebView::initialize();
#endif

    // Must be set before QGuiApplication so Controls never probe for
    // platform-specific styles (Windows / macOS / iOS).  Material is
    // available on every Qt platform including Android.
    QQuickStyle::setStyle("Material");

    QGuiApplication app(argc, argv);
    QCoreApplication::setOrganizationName("Lotly");
    QCoreApplication::setApplicationName("Lotly");

    qmlRegisterUncreatableType<ParkingLotModel>(
        "SmartParking.Backend", 1, 0, "ParkingLotModel",
        "ParkingLotModel is exposed by AppController.");

    qmlRegisterUncreatableType<SimulationManager>(
        "SmartParking.Backend", 1, 0, "SimulationManager",
        "SimulationManager is exposed by AppController.");

    qmlRegisterUncreatableType<SettingsManager>(
        "SmartParking.Backend", 1, 0, "SettingsManager",
        "SettingsManager is exposed via appController.settings.");

    AppController controller;

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("appController",   &controller);
    engine.rootContext()->setContextProperty("parkingLotModel", controller.parkingLotModel());
    // Keep settingsManager as a convenience alias so any legacy reference still compiles.
    engine.rootContext()->setContextProperty("settingsManager", controller.settings());
#ifdef HAS_QTWEBVIEW
    engine.rootContext()->setContextProperty("hasWebView", true);
#else
    engine.rootContext()->setContextProperty("hasWebView", false);
#endif

    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreationFailed,
        &app, []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    engine.loadFromModule("SmartParking", "Main");

    return app.exec();
}
