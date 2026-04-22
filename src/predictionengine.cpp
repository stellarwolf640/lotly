#include "predictionengine.h"

#include <QDateTime>
#include <QtGlobal>

#include "dataprocessor.h"
#include "lotreport.h"

// Default availability fraction (0–1) indexed by hour of day (0–23).
// Represents a typical campus parking day:
//   overnight → near-empty, morning rush → fills, midday → stays full, evening → clears.
// Replace with per-lot learned curves once enough historical data is available.
static constexpr double FALLBACK_CURVE[24] = {
    0.95, 0.95, 0.95, 0.95, 0.95, 0.90,  // 0–5 AM : overnight near-empty
    0.82, 0.68, 0.50, 0.38, 0.30, 0.28,  // 6–11 AM: morning rush fills
    0.30, 0.38, 0.40, 0.42, 0.40, 0.45,  // 12–5 PM: midday, some turnover
    0.55, 0.65, 0.74, 0.82, 0.88, 0.92   // 6–11 PM: evening clear-out
};

PredictionEngine::PredictionEngine(DataProcessor *dataProcessor)
    : m_dataProcessor(dataProcessor)
{
}

double PredictionEngine::fallbackFractionForHour(int hour)
{
    return FALLBACK_CURVE[qBound(0, hour, 23)];
}

int PredictionEngine::predictAvailableSpaces(const ParkingLot &lot,
                                              const QList<LotReport> &reports) const
{
    const int hour = QDateTime::currentDateTime().time().hour();
    const int fallbackSpaces =
        static_cast<int>(lot.totalSpaces() * fallbackFractionForHour(hour));

    if (!m_dataProcessor || reports.isEmpty())
        return fallbackSpaces;

    // Trust-weighted severity: 0.0 (empty) → 3.0 (full)
    const double severity = m_dataProcessor->reportSeverityWeighted(reports);
    const double occupancyFactor = severity / 3.0;
    const int dataSpaces = static_cast<int>(lot.totalSpaces() * (1.0 - occupancyFactor * 0.85));

    // Blend: more reports → more weight on data, fewer → more weight on fallback curve.
    // Full data-weight at MIN_DATA_WEIGHT trust points (e.g. 4 reports @ 0.75 each).
    const double trustWeight = m_dataProcessor->totalTrustWeight(reports);
    const double blend = qMin(1.0, trustWeight / MIN_DATA_WEIGHT);

    const int blended =
        static_cast<int>(dataSpaces * blend + fallbackSpaces * (1.0 - blend));

    return qMax(0, qMin(lot.totalSpaces(), blended));
}

QString PredictionEngine::buildExplanation(const ParkingLot &lot,
                                            const QList<LotReport> &reports) const
{
    const int hour = QDateTime::currentDateTime().time().hour();
    const int fallbackPct = static_cast<int>(fallbackFractionForHour(hour) * 100.0);

    if (!m_dataProcessor || reports.isEmpty()) {
        return QStringLiteral("No reports yet for %1. "
                              "Showing default %2% availability curve for hour %3.")
            .arg(lot.name())
            .arg(fallbackPct)
            .arg(hour);
    }

    const double trustWeight = m_dataProcessor->totalTrustWeight(reports);
    const double blend = qMin(1.0, trustWeight / MIN_DATA_WEIGHT);
    const int dataPct = qRound(blend * 100.0);

    if (blend < 0.5) {
        return QStringLiteral("%1 report(s) for %2 — data is limited. "
                              "Prediction is %3% fallback curve, %4% user reports.")
            .arg(reports.size())
            .arg(lot.name())
            .arg(100 - dataPct)
            .arg(dataPct);
    }

    return QStringLiteral("%1 report(s) for %2. "
                          "Prediction is %3% driven by recent user data.")
        .arg(reports.size())
        .arg(lot.name())
        .arg(dataPct);
}
