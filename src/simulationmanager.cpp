#include "simulationmanager.h"

#include <QDateTime>
#include <QMap>
#include <QRandomGenerator>
#include <QtGlobal>

#include "databasemanager.h"

SimulationManager::SimulationManager(DatabaseManager *databaseManager, QObject *parent)
    : QObject(parent)
    , m_db(databaseManager)
{
}

QString SimulationManager::lastSummary() const
{
    return m_lastSummary;
}

void SimulationManager::runSimulation(const QString &lotId,
                                       int startHour, int endHour,
                                       int reportCount,
                                       double minTrust, double maxTrust,
                                       const QStringList &statusWeights)
{
    if (!m_db || lotId.isEmpty() || reportCount <= 0) {
        m_lastSummary = QStringLiteral("Simulation aborted: invalid parameters.");
        emit lastSummaryChanged();
        return;
    }

    const int h0 = qBound(0, startHour, 23);
    const int h1 = qBound(h0 + 1, endHour, 24);
    const int windowSecs = (h1 - h0) * 3600;

    QMap<QString, int> counts;

    // Anchor the timeline to "today at startHour" so timestamps are meaningful
    QDateTime base = QDateTime::currentDateTime();
    base.setTime(QTime(h0, 0));

    for (int i = 0; i < reportCount; ++i) {
        const double trust  = randomInRange(qMax(0.1, minTrust), qMin(1.0, maxTrust));
        const LotReport::Status status = weightedRandomStatus(statusWeights);

        // Spread reports evenly across the window
        const int offsetSecs = (reportCount > 1)
                                    ? (i * windowSecs / (reportCount - 1))
                                    : 0;
        const QDateTime ts = base.addSecs(offsetSecs);

        // Simulated reports use "sim-N" user IDs so they can be selectively cleared
        const QString simUser = QStringLiteral("sim-%1").arg(i % 5);
        m_db->storeReport(LotReport(lotId, simUser, status, trust, ts));

        counts[LotReport::statusToString(status)]++;
    }

    QStringList parts;
    for (auto it = counts.constBegin(); it != counts.constEnd(); ++it)
        parts << QStringLiteral("%1\u00d7%2").arg(it.value()).arg(it.key());

    m_lastSummary = QStringLiteral("Injected %1 report(s) for \"%2\" (%3h\u2013%4h): %5")
                        .arg(reportCount).arg(lotId)
                        .arg(h0).arg(h1)
                        .arg(parts.join(QStringLiteral(", ")));
    emit lastSummaryChanged();
}

void SimulationManager::clearSimulation(const QString &lotId)
{
    if (!m_db) return;
    m_db->clearSimulatedReports(lotId);

    m_lastSummary = QStringLiteral("Cleared simulated reports for \"%1\". "
                                   "Trigger refresh to update predictions.").arg(lotId);
    emit lastSummaryChanged();
}

LotReport::Status SimulationManager::weightedRandomStatus(
    const QStringList &statusWeights) const
{
    // Build a flat weighted pool: "Moderate:3" → three entries of Moderate
    QList<QPair<LotReport::Status, int>> pool;
    int total = 0;

    for (const QString &entry : statusWeights) {
        const QStringList parts = entry.split(QLatin1Char(':'));
        if (parts.size() != 2) continue;
        const int w = parts[1].trimmed().toInt();
        if (w <= 0) continue;
        pool.append({LotReport::statusFromString(parts[0].trimmed()), w});
        total += w;
    }

    if (pool.isEmpty() || total <= 0)
        return LotReport::Status::Moderate;

    int roll = static_cast<int>(QRandomGenerator::global()->bounded(total));
    for (const auto &[status, weight] : pool) {
        roll -= weight;
        if (roll < 0) return status;
    }
    return LotReport::Status::Moderate;
}

double SimulationManager::randomInRange(double lo, double hi) const
{
    const double range = hi - lo;
    if (range <= 0.0) return lo;
    return lo + QRandomGenerator::global()->generateDouble() * range;
}
