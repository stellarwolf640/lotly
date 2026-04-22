#pragma once

#include <QList>
#include <QObject>
#include <QSqlDatabase>
#include <QString>

#include "lotreport.h"
#include "parkinglot.h"
#include "user.h"

class DatabaseManager : public QObject
{
    Q_OBJECT

public:
    explicit DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager() override;

    QList<ParkingLot> parkingLots() const;
    ParkingLot parkingLotById(const QString &lotId) const;

    // All reports for a lot, newest first
    QList<LotReport> reportsForLot(const QString &lotId) const;

    // Reports submitted since the given timestamp (for time-bucketed predictions)
    QList<LotReport> reportsForLotSince(const QString &lotId, const QDateTime &since) const;

    User activeUser() const;

    void storeReport(const LotReport &report);
    void replaceParkingLots(const QList<ParkingLot> &lots);
    void updateParkingLot(const ParkingLot &lot);

    // Removes only simulated reports (user_id LIKE 'sim-%') — safe to call mid-session
    void clearSimulatedReports(const QString &lotId);

    // Removes all reports for a lot — used when resetting a dev session
    void clearAllReportsForLot(const QString &lotId);

signals:
    void dataChanged();

private:
    void initSchema();
    void seedDefaultUser();

    QSqlDatabase m_db;
    User m_activeUser;
};
