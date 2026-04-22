#include "notificationmanager.h"

QString NotificationManager::buildReportPrompt(const QString &lotName) const
{
    return QStringLiteral("Leaving %1 soon? Share a quick parking status report to improve future recommendations.")
        .arg(lotName);
}
