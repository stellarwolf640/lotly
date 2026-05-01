#pragma once

#include <QDateTime>
#include <QString>

#include "lotreport.h"

class DatabaseManager;

class ReportManager
{
public:
    explicit ReportManager(DatabaseManager *databaseManager = nullptr);

    bool submitReport(const LotReport &report, QString *errorMessage = nullptr) const;

private:
    DatabaseManager *m_databaseManager = nullptr;
};
