#pragma once

#include <QString>

class NotificationManager
{
public:
    QString buildReportPrompt(const QString &lotName) const;
};
