#include "databasemanager.h"

#include <QDebug>
#include <algorithm>
#include <functional>

#include "firestoreservice.h"

// ── Construction ──────────────────────────────────────────────────────────────

DatabaseManager::DatabaseManager(FirestoreService *firestore, QObject *parent)
    : QObject(parent)
    , m_firestore(firestore)
{
    Q_ASSERT(m_firestore);
    connectFirestoreSignals();
}

void DatabaseManager::connectFirestoreSignals()
{
    // ── Parking lots ──────────────────────────────────────────────────────────
    connect(m_firestore, &FirestoreService::parkingLotsReceived, this,
            [this](const QList<ParkingLot> &lots) {
        m_lotsCache = lots;
        emit dataChanged();
    });

    // parkingLotStored — no action needed; cache is already up to date.

    // ── All-reports bulk fetch (startup) ──────────────────────────────────────
    connect(m_firestore, &FirestoreService::allReportsReceived, this,
            [this](const QList<LotReport> &reports) {
        // Group the flat list by lotId, replacing each lot's cache entry.
        // Locally-submitted reports that are not yet in Firestore were prepended
        // by storeReport() and are therefore newer (smaller index).  We keep
        // them by merging: if a local entry's timestamp is NOT present in the
        // fetched set, retain it.
        QMap<QString, QList<LotReport>> fetched;
        for (const LotReport &r : reports)
            fetched[r.lotId()].append(r);

        for (auto it = fetched.constBegin(); it != fetched.constEnd(); ++it) {
            const QString &lotId      = it.key();
            const QList<LotReport> &serverReports = it.value();

            // Collect locally-added reports whose timestamps are not in the
            // server set (they were submitted after the query was issued).
            QList<LotReport> localOnly;
            if (m_reportsCache.contains(lotId)) {
                for (const LotReport &local : m_reportsCache[lotId]) {
                    bool found = false;
                    for (const LotReport &srv : serverReports) {
                        if (srv.userId() == local.userId() &&
                            srv.timestamp() == local.timestamp()) {
                            found = true;
                            break;
                        }
                    }
                    if (!found)
                        localOnly.prepend(local);
                }
            }
            // localOnly are ordered newest-first; prepend them before server list
            m_reportsCache[lotId] = localOnly + serverReports;
        }

        m_reportsLoaded = true;
        emit reportsReadyChanged();
        emit dataChanged();
    });

    // ── Per-lot report fetch ───────────────────────────────────────────────────
    connect(m_firestore, &FirestoreService::reportsReceived, this,
            [this](const QString &lotId, const QList<LotReport> &reports) {
        m_reportsCache[lotId] = reports;
        emit dataChanged();
    });

    connect(m_firestore, &FirestoreService::reportStored, this,
            [this](const QString &firestoreDocId, const LotReport &report) {
        // Track the Firestore document ID for this report so we can delete it.
        const QString key = report.lotId() + QLatin1Char(':')
                          + report.userId() + QLatin1Char(':')
                          + report.timestamp().toString(Qt::ISODate);
        m_reportDocIds[key] = firestoreDocId;
    });

    // ── Users ─────────────────────────────────────────────────────────────────
    connect(m_firestore, &FirestoreService::userReceived, this,
            [this](const User &user) {
        m_activeUser = user;
        emit dataChanged();
    });

    // ── Errors ────────────────────────────────────────────────────────────────
    connect(m_firestore, &FirestoreService::errorOccurred, this,
            [](const QString &op, const QString &error) {
        qWarning() << "[DatabaseManager] Firestore error in" << op << ":" << error;
    });
}

// ── Synchronous reads ─────────────────────────────────────────────────────────

QList<ParkingLot> DatabaseManager::parkingLots() const
{
    return m_lotsCache;
}

ParkingLot DatabaseManager::parkingLotById(const QString &lotId) const
{
    for (const ParkingLot &lot : m_lotsCache) {
        if (lot.id() == lotId)
            return lot;
    }
    return {};
}

QList<LotReport> DatabaseManager::reportsForLot(const QString &lotId) const
{
    return m_reportsCache.value(lotId);
}

QList<LotReport> DatabaseManager::reportsForLotSince(const QString &lotId,
                                                      const QDateTime &since) const
{
    QList<LotReport> result;
    for (const LotReport &r : m_reportsCache.value(lotId)) {
        if (r.timestamp() >= since)
            result.append(r);
    }
    return result;
}

QList<LotReport> DatabaseManager::reportsForUser(const QString &userId) const
{
    QList<LotReport> result;
    for (auto it = m_reportsCache.constBegin(); it != m_reportsCache.constEnd(); ++it) {
        for (const LotReport &r : it.value()) {
            if (r.userId() == userId)
                result.append(r);
        }
    }
    // Most recent first.
    std::sort(result.begin(), result.end(), [](const LotReport &a, const LotReport &b) {
        return a.timestamp() > b.timestamp();
    });
    return result;
}

QDateTime DatabaseManager::lastReportTimeForUser(const QString &userId,
                                                  const QString &lotId) const
{
    QDateTime latest;
    for (const LotReport &r : m_reportsCache.value(lotId)) {
        if (r.userId() == userId) {
            if (!latest.isValid() || r.timestamp() > latest)
                latest = r.timestamp();
        }
    }
    return latest;
}

User DatabaseManager::activeUser() const
{
    return m_activeUser;
}

// ── Writes ────────────────────────────────────────────────────────────────────

void DatabaseManager::storeReport(const LotReport &report)
{
    // Update cache immediately so the prediction pipeline sees the new report.
    m_reportsCache[report.lotId()].prepend(report);
    emit dataChanged();

    // Async write — Firestore will emit reportStored with the generated doc ID.
    m_firestore->storeReport(report);
}

void DatabaseManager::replaceParkingLots(const QList<ParkingLot> &lots)
{
    m_lotsCache = lots;
    emit dataChanged();

    for (const ParkingLot &lot : lots)
        m_firestore->storeParkingLot(lot);
}

void DatabaseManager::updateParkingLot(const ParkingLot &lot)
{
    for (int i = 0; i < m_lotsCache.size(); ++i) {
        if (m_lotsCache[i].id() == lot.id()) {
            m_lotsCache[i] = lot;
            break;
        }
    }
    emit dataChanged();
    m_firestore->storeParkingLot(lot);
}

void DatabaseManager::clearSimulatedReports(const QString &lotId)
{
    removeReports(lotId, [](const LotReport &r) {
        return r.userId().startsWith(QStringLiteral("sim-"));
    });
}

void DatabaseManager::clearAllReportsForLot(const QString &lotId)
{
    removeReports(lotId, [](const LotReport &) { return true; });
}

void DatabaseManager::setActiveUser(const User &user)
{
    m_activeUser = user;
}

void DatabaseManager::fetchLotsFromFirestore()
{
    m_firestore->fetchParkingLots();
}

void DatabaseManager::fetchReportsFromFirestore(const QString &lotId)
{
    m_firestore->fetchReportsForLot(lotId);
}

void DatabaseManager::refreshReports()
{
    m_firestore->fetchAllReports();
}

bool DatabaseManager::reportsLoaded() const
{
    return m_reportsLoaded;
}

// ── Private helpers ───────────────────────────────────────────────────────────

void DatabaseManager::removeReports(const QString &lotId,
                                     std::function<bool(const LotReport &)> predicate)
{
    QList<LotReport> &reports = m_reportsCache[lotId];
    QList<LotReport>  kept;

    for (const LotReport &r : reports) {
        if (predicate(r)) {
            // Look up the Firestore document ID and delete remotely.
            const QString key = lotId + QLatin1Char(':')
                              + r.userId() + QLatin1Char(':')
                              + r.timestamp().toString(Qt::ISODate);
            const QString docId = m_reportDocIds.take(key);
            if (!docId.isEmpty())
                m_firestore->deleteDocument(QStringLiteral("lot_reports"), docId);
        } else {
            kept.append(r);
        }
    }

    reports = kept;
    // No dataChanged here — caller (SimulationManager) decides when to refresh.
}
