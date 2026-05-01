#include "dataprocessor.h"

#include <QtGlobal>

// ── Age-decay lookup ──────────────────────────────────────────────────────────
// Returns a multiplier (0–1) based on how old a report is and which calendar
// day it came from, relative to now.
static double ageDecay(const LotReport &report, const QDateTime &now)
{
    if (!report.timestamp().isValid() || report.timestamp() > now)
        return 0.0;

    const double elapsedHours =
        static_cast<double>(report.timestamp().secsTo(now)) / 3600.0;

    // Within the last 1–3 hours: fine-grained decay.
    if (elapsedHours <= 1.0) return 1.00;
    if (elapsedHours <= 2.0) return 0.80;
    if (elapsedHours <= 3.0) return 0.60;

    // Older — fall back to calendar-day granularity.
    const int daysDiff =
        static_cast<int>(report.timestamp().date().daysTo(now.date()));

    if (daysDiff == 0) return 0.30;   // today, > 3 hours old
    if (daysDiff == 1) return 0.15;   // yesterday
    if (daysDiff == 2) return 0.08;   // 2 days ago
    return 0.03;                       // older than 2 days
}

// ── Hour-proximity lookup ─────────────────────────────────────────────────────
// A report submitted at hour R contributes to the prediction for target hour T.
// Handles 24-hour wrap-around (e.g. report at 23:00, target = 01:00 → diff = 2).
static double hourProximity(int reportHour, int targetHour)
{
    int diff = qAbs(targetHour - reportHour);
    if (diff > 12)
        diff = 24 - diff;   // e.g. |1 - 23| = 22 → 24 - 22 = 2

    if (diff == 0) return 1.00;
    if (diff == 1) return 0.60;
    if (diff == 2) return 0.30;
    return 0.10;
}

// ── Public API ────────────────────────────────────────────────────────────────

double DataProcessor::computeReportWeight(const LotReport &report,
                                          int targetHour,
                                          const QDateTime &now) const
{
    const double ad = ageDecay(report, now);
    if (ad <= 0.0)
        return 0.0;

    const int reportHour = report.timestamp().time().hour();
    const double hp      = hourProximity(reportHour, targetHour);

    // Trust score: 0.0 (completely unreliable) to 1.0 (fully trusted).
    // Default LotReport trustScore is 0.75; simulated reports also use this.
    return ad * hp * qMax(0.0, report.trustScore());
}

double DataProcessor::reportSeverityTimeWeighted(const QList<LotReport> &reports,
                                                  int targetHour,
                                                  const QDateTime &now) const
{
    if (reports.isEmpty())
        return -1.0;

    double weightedSum = 0.0;
    double totalWeight = 0.0;

    for (const LotReport &r : reports) {
        const double w = computeReportWeight(r, targetHour, now);
        if (w <= 0.0)
            continue;

        // Normalise Status enum (0–3) to severity fraction (0.0–1.0).
        const double severity =
            static_cast<double>(static_cast<int>(r.status())) / 3.0;

        weightedSum += severity * w;
        totalWeight += w;
    }

    if (totalWeight <= 0.0)
        return -1.0;   // no meaningful data for this hour

    return weightedSum / totalWeight;   // 0.0 = empty, 1.0 = full
}

double DataProcessor::totalTimeWeight(const QList<LotReport> &reports,
                                      int targetHour,
                                      const QDateTime &now) const
{
    double total = 0.0;
    for (const LotReport &r : reports)
        total += computeReportWeight(r, targetHour, now);
    return total;
}
