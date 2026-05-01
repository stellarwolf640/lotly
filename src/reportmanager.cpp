#include "reportmanager.h"

#include "databasemanager.h"

// A user may submit at most one report per lot within this window.
static constexpr int kCooldownMinutes = 30;

ReportManager::ReportManager(DatabaseManager *databaseManager)
    : m_databaseManager(databaseManager)
{
}

bool ReportManager::submitReport(const LotReport &report, QString *errorMessage) const
{
    if (!m_databaseManager) {
        if (errorMessage)
            *errorMessage = QStringLiteral("Database manager is not configured.");
        return false;
    }

    if (report.lotId().isEmpty()) {
        if (errorMessage)
            *errorMessage = QStringLiteral("A parking lot must be selected before submitting a report.");
        return false;
    }

    if (report.userId().isEmpty()) {
        if (errorMessage)
            *errorMessage = QStringLiteral("A user identifier is required for report submission.");
        return false;
    }

    // ── Per-user per-lot cooldown ─────────────────────────────────────────────
    // Check against the in-memory cache — no network call needed.
    const QDateTime lastReport =
        m_databaseManager->lastReportTimeForUser(report.userId(), report.lotId());
    if (lastReport.isValid()) {
        const qint64 elapsedSecs = lastReport.secsTo(QDateTime::currentDateTime());
        const qint64 cooldownSecs = static_cast<qint64>(kCooldownMinutes) * 60;
        if (elapsedSecs < cooldownSecs) {
            const int remainingMins =
                static_cast<int>((cooldownSecs - elapsedSecs + 59) / 60);
            if (errorMessage) {
                *errorMessage =
                    QStringLiteral("You already reported this lot recently. "
                                   "Please wait %1 minute%2 before reporting again.")
                        .arg(remainingMins)
                        .arg(remainingMins == 1 ? QString() : QStringLiteral("s"));
            }
            return false;
        }
    }

    m_databaseManager->storeReport(report);
    return true;
}
