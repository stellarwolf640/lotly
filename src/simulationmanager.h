#pragma once

#include <QMap>
#include <QObject>
#include <QStringList>

#include "lotreport.h"

class DatabaseManager;

// SimulationManager generates synthetic LotReports and injects them into the
// DatabaseManager so the prediction pipeline can be exercised without real users.
//
// QML usage example:
//   appController.simulationManager.runSimulation(
//       "north-garage",   // lotId
//       8, 14,            // startHour, endHour
//       20,               // reportCount
//       0.4, 0.9,         // minTrust, maxTrust
//       ["Empty:1","Moderate:3","NearlyFull:2","Full:1"]  // weighted status distribution
//   )
class SimulationManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString lastSummary READ lastSummary NOTIFY lastSummaryChanged)

public:
    explicit SimulationManager(DatabaseManager *databaseManager,
                               QObject *parent = nullptr);

    QString lastSummary() const;

    // Generate reportCount synthetic reports for lotId, spread across [startHour, endHour).
    // Trust scores are drawn uniformly from [minTrust, maxTrust].
    // statusWeights is a list of "StatusName:weight" strings (e.g. "Moderate:3").
    Q_INVOKABLE void runSimulation(const QString &lotId,
                                   int startHour, int endHour,
                                   int reportCount,
                                   double minTrust, double maxTrust,
                                   const QStringList &statusWeights);

    // Remove only simulated reports (tagged user_id = "sim-*") for a lot
    Q_INVOKABLE void clearSimulation(const QString &lotId);

signals:
    void lastSummaryChanged();

private:
    LotReport::Status weightedRandomStatus(const QStringList &statusWeights) const;
    double randomInRange(double lo, double hi) const;

    DatabaseManager *m_db = nullptr;
    QString m_lastSummary;
};
