#pragma once

#include <QList>

#include "parkinglot.h"

class DataProcessor;
class LotReport;

class PredictionEngine
{
public:
    explicit PredictionEngine(DataProcessor *dataProcessor = nullptr);

    int predictAvailableSpaces(const ParkingLot &lot, const QList<LotReport> &reports) const;
    QString buildExplanation(const ParkingLot &lot, const QList<LotReport> &reports) const;

    // Returns the default availability fraction (0–1) for a given hour of day (0–23).
    // Used as a prior when real report data is sparse.
    static double fallbackFractionForHour(int hour);

private:
    DataProcessor *m_dataProcessor = nullptr;

    // Minimum total trust-weight before report data fully dominates the fallback curve
    static constexpr double MIN_DATA_WEIGHT = 3.0;
};
