#pragma once

#include <QVariantMap>

class DatabaseManager;

class RecommendationEngine
{
public:
    explicit RecommendationEngine(DatabaseManager *databaseManager = nullptr);

    QVariantMap buildRecommendation() const;

private:
    DatabaseManager *m_databaseManager = nullptr;
};
