#pragma once

#include <QDateTime>
#include <QList>
#include <QMap>
#include <QObject>
#include <QString>

#include "lotreport.h"
#include "parkinglot.h"
#include "user.h"

class FirestoreService;

// DatabaseManager maintains in-memory caches of all domain objects and
// synchronises them with Cloud Firestore asynchronously via FirestoreService.
//
// Synchronous getters always return from the in-memory cache.
// Writes update the cache immediately and fire async Firestore writes.
// When Firestore returns data (fetch responses), the cache is updated
// and dataChanged() is emitted so the UI re-renders.
//
// The public API (method names and signatures) is preserved so the rest of
// the app compiles without changes.
class DatabaseManager : public QObject
{
    Q_OBJECT

public:
    explicit DatabaseManager(FirestoreService *firestore,
                             QObject *parent = nullptr);

    // ── Synchronous reads (from in-memory cache) ──────────────────────────────
    QList<ParkingLot> parkingLots() const;
    ParkingLot        parkingLotById(const QString &lotId) const;

    QList<LotReport>  reportsForLot(const QString &lotId) const;
    QList<LotReport>  reportsForLotSince(const QString &lotId,
                                         const QDateTime &since) const;

    // Returns all reports submitted by userId across every lot,
    // sorted by timestamp descending (most recent first).
    // Reads from the in-memory cache only; newly submitted reports
    // are visible immediately because storeReport() prepends them.
    QList<LotReport>  reportsForUser(const QString &userId) const;

    // Returns the timestamp of the most recent report by userId for lotId,
    // or an invalid QDateTime if no such report exists in the cache.
    // Used by ReportManager for instant (no-network) cooldown enforcement.
    QDateTime lastReportTimeForUser(const QString &userId, const QString &lotId) const;

    User activeUser() const;

    // ── Writes (update cache + async Firestore write) ─────────────────────────
    void storeReport(const LotReport &report);
    void replaceParkingLots(const QList<ParkingLot> &lots);
    void updateParkingLot(const ParkingLot &lot);

    // Removes only simulated reports (userId.startsWith("sim-")) for the lot.
    void clearSimulatedReports(const QString &lotId);

    // Removes all reports for the lot.
    void clearAllReportsForLot(const QString &lotId);

    // Called by AppController when the signed-in user changes.
    void setActiveUser(const User &user);

    // Trigger an async fetch of parking lots from Firestore.
    void fetchLotsFromFirestore();

    // Trigger an async fetch of reports for a specific lot.
    void fetchReportsFromFirestore(const QString &lotId);

    // Trigger a full cross-lot report fetch (called on startup, after lots load).
    // Emits reportsLoaded() then dataChanged() when the fetch completes.
    void refreshReports();

    // True once at least one fetchAllReports() response has been processed.
    bool reportsLoaded() const;

signals:
    void dataChanged();

    // Emitted once after the initial fetchAllReports() response is processed.
    // AppController uses this to clear the dataLoading spinner.
    void reportsReadyChanged();

private:
    void connectFirestoreSignals();

    // Removes reports matching a predicate from cache and Firestore.
    void removeReports(const QString &lotId,
                       std::function<bool(const LotReport &)> predicate);

    FirestoreService *m_firestore;

    // In-memory caches
    QList<ParkingLot>                    m_lotsCache;
    QMap<QString, QList<LotReport>>      m_reportsCache;   // lotId → reports
    QMap<QString, QString>               m_reportDocIds;   // "lotId:userId:ts" → firestoreDocId
    User                                 m_activeUser;
    bool                                 m_reportsLoaded = false;
};
