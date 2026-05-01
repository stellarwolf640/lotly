#pragma once

#include <QList>
#include <QVariantList>

#include "parkinglot.h"

class DataProcessor;
class LotReport;

// PredictionEngine blends time-weighted report data with a statistical fallback
// curve to predict parking availability.
//
// Blending strategy (based on DataProcessor::totalTimeWeight):
//   ≥ 3.0  →  90% real data / 10% fallback
//   ≥ 1.0  →  70% real data / 30% fallback
//   ≥ 0.3  →  40% real data / 60% fallback
//   < 0.3  → 100% fallback  (data too old or nonexistent)
//
// Severity scale from DataProcessor: 0.0 = empty, 1.0 = completely full.
// Availability fraction returned here: 0.0 = no spaces, 1.0 = all spaces free.
class PredictionEngine
{
public:
    explicit PredictionEngine(DataProcessor *dataProcessor = nullptr);

    // Predicts available spaces for the current hour using blended data.
    int predictAvailableSpaces(const ParkingLot &lot,
                               const QList<LotReport> &reports) const;

    // Human-readable explanation of the current prediction blend.
    QString buildExplanation(const ParkingLot &lot,
                             const QList<LotReport> &reports) const;

    // Returns 21 hourly availability values (hours 6 AM through 2 AM next day),
    // each an integer 0–100 representing predicted % of spaces that are free.
    // Used by AppController::lotToVariantMap() to populate "hourlyAvailability".
    QVariantList buildHourlyPredictions(const ParkingLot &lot,
                                        const QList<LotReport> &reports) const;

    // Returns the statistical fallback availability fraction (0–1) for hour 0–23.
    // Public so AppController can use it for lots with no report data at all.
    static double fallbackFractionForHour(int hour);

private:
    DataProcessor *m_dataProcessor = nullptr;
};
