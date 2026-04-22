#include "databasemanager.h"

#include <QDebug>
#include <QDir>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>

DatabaseManager::DatabaseManager(QObject *parent)
    : QObject(parent)
{
    // Store the SQLite file under the OS app-data directory so reports survive restarts.
    // To wipe all data during development, delete the lotly.db file.
    const QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dataPath);

    m_db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"));
    m_db.setDatabaseName(dataPath + QStringLiteral("/lotly.db"));

    if (!m_db.open()) {
        qWarning() << "DatabaseManager: cannot open SQLite database:" << m_db.lastError().text();
        return;
    }

    // Enable WAL mode for better concurrent write performance
    QSqlQuery(QStringLiteral("PRAGMA journal_mode=WAL"), m_db);

    initSchema();
    seedDefaultUser();
}

DatabaseManager::~DatabaseManager()
{
    if (m_db.isOpen())
        m_db.close();
}

// ── Schema ────────────────────────────────────────────────────────────────────

void DatabaseManager::initSchema()
{
    QSqlQuery q(m_db);

    q.exec(R"(
        CREATE TABLE IF NOT EXISTS parking_lots (
            id            TEXT PRIMARY KEY,
            name          TEXT NOT NULL,
            location      TEXT,
            latitude      REAL DEFAULT 0.0,
            longitude     REAL DEFAULT 0.0,
            place_id      TEXT DEFAULT '',
            total_spaces  INTEGER DEFAULT 0,
            predicted     INTEGER DEFAULT 0,
            confidence    REAL DEFAULT 0.35,
            explanation   TEXT DEFAULT ''
        )
    )");

    q.exec(R"(
        CREATE TABLE IF NOT EXISTS reports (
            rowid       INTEGER PRIMARY KEY AUTOINCREMENT,
            lot_id      TEXT NOT NULL,
            user_id     TEXT NOT NULL,
            status      INTEGER NOT NULL,
            trust_score REAL DEFAULT 0.75,
            timestamp   TEXT NOT NULL
        )
    )");

    q.exec(R"(
        CREATE TABLE IF NOT EXISTS users (
            id                TEXT PRIMARY KEY,
            display_name      TEXT,
            reliability_score REAL DEFAULT 0.75
        )
    )");

    // Speed up time-windowed queries used by PredictionEngine
    q.exec("CREATE INDEX IF NOT EXISTS idx_reports_lot_time ON reports(lot_id, timestamp)");
}

void DatabaseManager::seedDefaultUser()
{
    QSqlQuery q(m_db);
    q.prepare(R"(INSERT OR IGNORE INTO users (id, display_name, reliability_score) VALUES (?,?,?))");
    q.addBindValue(QStringLiteral("demo-user"));
    q.addBindValue(QStringLiteral("Campus Driver"));
    q.addBindValue(0.82);
    q.exec();

    m_activeUser = User(QStringLiteral("demo-user"), QStringLiteral("Campus Driver"), 0.82);
}

// ── Reads ─────────────────────────────────────────────────────────────────────

QList<ParkingLot> DatabaseManager::parkingLots() const
{
    QList<ParkingLot> lots;
    QSqlQuery q(QStringLiteral(
        "SELECT id,name,location,latitude,longitude,place_id,"
        "total_spaces,predicted,confidence,explanation FROM parking_lots"), m_db);

    while (q.next()) {
        lots.append(ParkingLot(
            q.value(0).toString(), q.value(1).toString(), q.value(2).toString(),
            q.value(3).toDouble(),  q.value(4).toDouble(),  q.value(5).toString(),
            q.value(6).toInt(),     q.value(7).toInt(),
            q.value(8).toDouble(),  q.value(9).toString()));
    }
    return lots;
}

ParkingLot DatabaseManager::parkingLotById(const QString &lotId) const
{
    QSqlQuery q(m_db);
    q.prepare(QStringLiteral(
        "SELECT id,name,location,latitude,longitude,place_id,"
        "total_spaces,predicted,confidence,explanation FROM parking_lots WHERE id=?"));
    q.addBindValue(lotId);
    q.exec();

    if (q.next()) {
        return ParkingLot(
            q.value(0).toString(), q.value(1).toString(), q.value(2).toString(),
            q.value(3).toDouble(),  q.value(4).toDouble(),  q.value(5).toString(),
            q.value(6).toInt(),     q.value(7).toInt(),
            q.value(8).toDouble(),  q.value(9).toString());
    }
    return {};
}

QList<LotReport> DatabaseManager::reportsForLot(const QString &lotId) const
{
    QList<LotReport> results;
    QSqlQuery q(m_db);
    q.prepare(QStringLiteral(
        "SELECT lot_id,user_id,status,trust_score,timestamp FROM reports "
        "WHERE lot_id=? ORDER BY timestamp DESC"));
    q.addBindValue(lotId);
    q.exec();

    while (q.next()) {
        results.append(LotReport(
            q.value(0).toString(), q.value(1).toString(),
            static_cast<LotReport::Status>(q.value(2).toInt()),
            q.value(3).toDouble(),
            QDateTime::fromString(q.value(4).toString(), Qt::ISODate)));
    }
    return results;
}

QList<LotReport> DatabaseManager::reportsForLotSince(const QString &lotId,
                                                      const QDateTime &since) const
{
    QList<LotReport> results;
    QSqlQuery q(m_db);
    q.prepare(QStringLiteral(
        "SELECT lot_id,user_id,status,trust_score,timestamp FROM reports "
        "WHERE lot_id=? AND timestamp>=? ORDER BY timestamp DESC"));
    q.addBindValue(lotId);
    q.addBindValue(since.toString(Qt::ISODate));
    q.exec();

    while (q.next()) {
        results.append(LotReport(
            q.value(0).toString(), q.value(1).toString(),
            static_cast<LotReport::Status>(q.value(2).toInt()),
            q.value(3).toDouble(),
            QDateTime::fromString(q.value(4).toString(), Qt::ISODate)));
    }
    return results;
}

User DatabaseManager::activeUser() const
{
    return m_activeUser;
}

// ── Writes ────────────────────────────────────────────────────────────────────

void DatabaseManager::storeReport(const LotReport &report)
{
    QSqlQuery q(m_db);
    q.prepare(QStringLiteral(
        "INSERT INTO reports (lot_id,user_id,status,trust_score,timestamp) VALUES (?,?,?,?,?)"));
    q.addBindValue(report.lotId());
    q.addBindValue(report.userId());
    q.addBindValue(static_cast<int>(report.status()));
    q.addBindValue(report.trustScore());
    q.addBindValue(report.timestamp().toString(Qt::ISODate));

    if (!q.exec())
        qWarning() << "DatabaseManager: storeReport failed:" << q.lastError().text();
    else
        emit dataChanged();
}

void DatabaseManager::replaceParkingLots(const QList<ParkingLot> &lots)
{
    QSqlQuery q(m_db);
    q.exec(QStringLiteral("DELETE FROM parking_lots"));

    for (const ParkingLot &lot : lots) {
        q.prepare(QStringLiteral(R"(
            INSERT OR REPLACE INTO parking_lots
            (id,name,location,latitude,longitude,place_id,total_spaces,predicted,confidence,explanation)
            VALUES (?,?,?,?,?,?,?,?,?,?)
        )"));
        q.addBindValue(lot.id());
        q.addBindValue(lot.name());
        q.addBindValue(lot.location());
        q.addBindValue(lot.latitude());
        q.addBindValue(lot.longitude());
        q.addBindValue(lot.placeId());
        q.addBindValue(lot.totalSpaces());
        q.addBindValue(lot.predictedAvailableSpaces());
        q.addBindValue(lot.confidence());
        q.addBindValue(lot.explanation());
        if (!q.exec())
            qWarning() << "DatabaseManager: replaceParkingLots insert failed:" << q.lastError().text();
    }

    emit dataChanged();
}

void DatabaseManager::updateParkingLot(const ParkingLot &lot)
{
    QSqlQuery q(m_db);
    q.prepare(QStringLiteral(R"(
        UPDATE parking_lots
        SET name=?,location=?,latitude=?,longitude=?,place_id=?,
            total_spaces=?,predicted=?,confidence=?,explanation=?
        WHERE id=?
    )"));
    q.addBindValue(lot.name());
    q.addBindValue(lot.location());
    q.addBindValue(lot.latitude());
    q.addBindValue(lot.longitude());
    q.addBindValue(lot.placeId());
    q.addBindValue(lot.totalSpaces());
    q.addBindValue(lot.predictedAvailableSpaces());
    q.addBindValue(lot.confidence());
    q.addBindValue(lot.explanation());
    q.addBindValue(lot.id());

    if (!q.exec())
        qWarning() << "DatabaseManager: updateParkingLot failed:" << q.lastError().text();
    else
        emit dataChanged();
}

void DatabaseManager::clearSimulatedReports(const QString &lotId)
{
    QSqlQuery q(m_db);
    q.prepare(QStringLiteral("DELETE FROM reports WHERE lot_id=? AND user_id LIKE 'sim-%'"));
    q.addBindValue(lotId);
    q.exec();
    // No dataChanged — caller decides when to re-run refreshLots
}

void DatabaseManager::clearAllReportsForLot(const QString &lotId)
{
    QSqlQuery q(m_db);
    q.prepare(QStringLiteral("DELETE FROM reports WHERE lot_id=?"));
    q.addBindValue(lotId);
    q.exec();
}
