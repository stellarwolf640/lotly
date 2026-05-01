#include "predictionengine.h"

#include <QDateTime>
#include <QtGlobal>

#include "dataprocessor.h"
#include "lotreport.h"

// ── Fallback curve ────────────────────────────────────────────────────────────
// Default availability fraction (0–1) indexed by hour of day (0–23).
// Represents a typical campus parking day:
//   0–5 AM  : late night / pre-dawn — nearly empty → high availability.
//   6–11 AM : morning rush — fills quickly; peak busy at 9–10 AM.
//   12–5 PM : midday peak around 1–2 PM, eases from 4 PM onward.
//   6–11 PM : evening clear-out — availability grows quickly.
static constexpr double FALLBACK_CURVE[24] = {
    0.97, 0.98, 0.99, 0.99, 0.99, 0.99,  // 0–5 AM
    0.85, 0.75, 0.55, 0.30, 0.25, 0.28,  // 6–11 AM
    0.35, 0.30, 0.28, 0.35, 0.45, 0.60,  // 12–5 PM
    0.75, 0.82, 0.87, 0.90, 0.93, 0.95   // 6–11 PM
};

// ── Construction ──────────────────────────────────────────────────────────────

PredictionEngine::PredictionEngine(DataProcessor *dataProcessor)
    : m_dataProcessor(dataProcessor)
{
}

// ── Static helpers ────────────────────────────────────────────────────────────

double PredictionEngine::fallbackFractionForHour(int hour)
{
    return FALLBACK_CURVE[qBound(0, hour, 23)];
}

// Blend real-data availability fraction with the fallback curve.
//   severity   : 0.0 = empty (all spaces free) → 1.0 = full.
//   timeWeight : from DataProcessor::totalTimeWeight().
//   fallback   : FALLBACK_CURVE value for the target hour.
// Returns availability fraction: 0.0 = no spaces, 1.0 = all spaces free.
static double blendAvailability(double severity, double timeWeight, double fallback)
{
    const double real = 1.0 - severity;   // invert severity → availability

    if (timeWeight >= 3.0) return real * 0.9 + fallback * 0.1;
    if (timeWeight >= 1.0) return real * 0.7 + fallback * 0.3;
    if (timeWeight >= 0.3) return real * 0.4 + fallback * 0.6;
    return fallback;
}

// ── Public API ────────────────────────────────────────────────────────────────

int PredictionEngine::predictAvailableSpaces(const ParkingLot &lot,
                                              const QList<LotReport> &reports) const
{
    const QDateTime now  = QDateTime::currentDateTime();
    const int       hour = now.time().hour();
    const double    fb   = fallbackFractionForHour(hour);

    if (!m_dataProcessor || reports.isEmpty())
        return static_cast<int>(lot.totalSpaces() * fb);

    const double severity   = m_dataProcessor->reportSeverityTimeWeighted(reports, hour, now);
    const double timeWeight = m_dataProcessor->totalTimeWeight(reports, hour, now);

    // severity < 0 means no reports have meaningful weight for this hour.
    const double fraction = (severity >= 0.0)
        ? blendAvailability(severity, timeWeight, fb)
        : fb;

    return qMax(0, qMin(lot.totalSpaces(),
                        static_cast<int>(lot.totalSpaces() * fraction)));
}

QString PredictionEngine::buildExplanation(const ParkingLot &lot,
                                            const QList<LotReport> &reports) const
{
    const QDateTime now  = QDateTime::currentDateTime();
    const int       hour = now.time().hour();

    if (!m_dataProcessor || reports.isEmpty()) {
        return QStringLiteral("No reports yet for %1. Showing predicted availability.")
            .arg(lot.name());
    }

    const double timeWeight = m_dataProcessor->totalTimeWeight(reports, hour, now);

    if (timeWeight < 0.3) {
        return QStringLiteral("%1 report(s) for %2 — data is too old to be reliable. "
                              "Showing predicted availability.")
            .arg(reports.size())
            .arg(lot.name());
    }

    int dataPct;
    if (timeWeight >= 3.0)      dataPct = 90;
    else if (timeWeight >= 1.0) dataPct = 70;
    else                        dataPct = 40;

    return QStringLiteral("%1 recent report(s) for %2. "
                          "Prediction is %3% driven by user data.")
        .arg(reports.size())
        .arg(lot.name())
        .arg(dataPct);
}

QVariantList PredictionEngine::buildHourlyPredictions(const ParkingLot &lot,
                                                       const QList<LotReport> &reports) const
{
    Q_UNUSED(lot)

    const QDateTime now = QDateTime::currentDateTime();
    QVariantList result;
    result.reserve(21);

    // Hours 6, 7, …, 23, 0, 1, 2 (21 values total).
    for (int i = 0; i < 21; ++i) {
        const int    h  = (6 + i) % 24;
        const double fb = fallbackFractionForHour(h);

        double fraction = fb;   // default: pure fallback

        if (m_dataProcessor && !reports.isEmpty()) {
            const double severity   = m_dataProcessor->reportSeverityTimeWeighted(reports, h, now);
            const double timeWeight = m_dataProcessor->totalTimeWeight(reports, h, now);

            if (severity >= 0.0)
                fraction = blendAvailability(severity, timeWeight, fb);
        }

        result.append(qRound(fraction * 100.0));
    }

    return result;
}
