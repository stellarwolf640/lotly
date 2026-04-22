#include "recommendationengine.h"

#include <algorithm>
#include <cmath>
#include <QtMath>

#include "databasemanager.h"
#include "parkinglot.h"

RecommendationEngine::RecommendationEngine(DatabaseManager *databaseManager)
    : m_databaseManager(databaseManager)
{
}

QVariantMap RecommendationEngine::buildRecommendation() const
{
    QVariantMap response;
    if (!m_databaseManager) {
        return response;
    }

    QList<ParkingLot> lots = m_databaseManager->parkingLots();

    // Ranking priority: 1. confidence DESC  2. percentOpen DESC  3. distance ASC
    std::sort(lots.begin(), lots.end(), [](const ParkingLot &a, const ParkingLot &b) {
        constexpr double CONF_EPSILON = 0.05;
        if (std::abs(a.confidence() - b.confidence()) > CONF_EPSILON)
            return a.confidence() > b.confidence();

        const int pctA = a.totalSpaces() > 0
            ? static_cast<int>(static_cast<double>(a.predictedAvailableSpaces()) / a.totalSpaces() * 100)
            : 0;
        const int pctB = b.totalSpaces() > 0
            ? static_cast<int>(static_cast<double>(b.predictedAvailableSpaces()) / b.totalSpaces() * 100)
            : 0;
        if (pctA != pctB)
            return pctA > pctB;

        // Both distances unknown (-1) are treated as equal; known < unknown
        if (a.distanceMeters() < 0 && b.distanceMeters() >= 0) return false;
        if (a.distanceMeters() >= 0 && b.distanceMeters() < 0) return true;
        return a.distanceMeters() < b.distanceMeters();
    });

    if (!lots.isEmpty()) {
        const ParkingLot &best = lots.at(0);
        const int bestPercentOpen = best.totalSpaces() > 0
                                        ? qRound((static_cast<double>(best.predictedAvailableSpaces())
                                                  / best.totalSpaces())
                                                 * 100.0)
                                        : 0;
        response.insert(QStringLiteral("bestLotId"), best.id());
        response.insert(QStringLiteral("bestLotName"), best.name());
        response.insert(QStringLiteral("bestLotLocation"), best.location());
        response.insert(QStringLiteral("bestLotAvailability"), best.predictedAvailableSpaces());
        response.insert(QStringLiteral("bestLotPercentOpen"), bestPercentOpen);
        response.insert(QStringLiteral("bestLotConfidence"), best.confidence());
    }

    if (lots.size() > 1) {
        const ParkingLot &backup = lots.at(1);
        const int backupPercentOpen = backup.totalSpaces() > 0
                                          ? qRound((static_cast<double>(backup.predictedAvailableSpaces())
                                                    / backup.totalSpaces())
                                                   * 100.0)
                                          : 0;
        response.insert(QStringLiteral("backupLotId"), backup.id());
        response.insert(QStringLiteral("backupLotName"), backup.name());
        response.insert(QStringLiteral("backupLotLocation"), backup.location());
        response.insert(QStringLiteral("backupLotAvailability"), backup.predictedAvailableSpaces());
        response.insert(QStringLiteral("backupLotPercentOpen"), backupPercentOpen);
        response.insert(QStringLiteral("backupLotConfidence"), backup.confidence());
    }

    response.insert(QStringLiteral("explanation"),
                    QStringLiteral("Ranked by confidence, then availability, then distance from your location."));

    return response;
}
