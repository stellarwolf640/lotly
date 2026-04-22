#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtQml/qqml.h>
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

    QGuiApplication app(argc, argv);
    QCoreApplication::setOrganizationName("Lotly");
    QCoreApplication::setApplicationName("Smart Parking Availability & Prediction System");

    qmlRegisterUncreatableType<ParkingLotModel>(
        "SmartParking.Backend", 1, 0, "ParkingLotModel",
        "ParkingLotModel is exposed by AppController.");

    qmlRegisterUncreatableType<SimulationManager>(
        "SmartParking.Backend", 1, 0, "SimulationManager",
        "SimulationManager is exposed by AppController.");

    AppController   controller;
    SettingsManager settings;

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("appController",   &controller);
    engine.rootContext()->setContextProperty("parkingLotModel", controller.parkingLotModel());
    engine.rootContext()->setContextProperty("settingsManager", &settings);
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
