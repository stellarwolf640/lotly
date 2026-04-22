#include "reportmanager.h"

#include "databasemanager.h"

ReportManager::ReportManager(DatabaseManager *databaseManager)
    : m_databaseManager(databaseManager)
{
}

bool ReportManager::submitReport(const LotReport &report, QString *errorMessage) const
{
    if (!m_databaseManager) {
        if (errorMessage) {
            *errorMessage = QStringLiteral("Database manager is not configured.");
        }
        return false;
    }

    if (report.lotId().isEmpty()) {
        if (errorMessage) {
            *errorMessage = QStringLiteral("A parking lot must be selected before submitting a report.");
        }
        return false;
    }

    if (report.userId().isEmpty()) {
        if (errorMessage) {
            *errorMessage = QStringLiteral("A user identifier is required for report submission.");
        }
        return false;
    }

    m_databaseManager->storeReport(report);
    return true;
}
