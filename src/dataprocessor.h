#pragma once

#include <QDateTime>
#include <QList>

#include "lotreport.h"

// DataProcessor produces weighted statistics from a list of LotReports.
//
// Weight for a report is the product of three factors:
//   1. Age decay     — how recently the report was submitted (vs now)
//   2. Hour proximity — how close the report's hour is to the target hour
//   3. Trust score   — submitter reliability (0–1 from User.reliabilityScore)
//
// Severity scale: 0.0 = lot empty, 1.0 = lot completely full.
// The Status enum maps as: Empty→0.00, Moderate→0.33, NearlyFull→0.67, Full→1.00
class DataProcessor
{
public:
    // ── Core weight computation ───────────────────────────────────────────────

    // Returns the combined weight for one report when predicting targetHour.
    // Returns 0 if the report timestamp is in the future or invalid.
    double computeReportWeight(const LotReport &report,
                               int targetHour,
                               const QDateTime &now) const;

    // ── Aggregates ────────────────────────────────────────────────────────────

    // Weighted-average severity (0.0–1.0) for targetHour.
    // Returns -1.0 when totalTimeWeight() < a meaningful threshold (no data).
    double reportSeverityTimeWeighted(const QList<LotReport> &reports,
                                      int targetHour,
                                      const QDateTime &now) const;

    // Sum of all report weights for targetHour.
    // Used by PredictionEngine (blend factor) and ConfidenceCalculator.
    double totalTimeWeight(const QList<LotReport> &reports,
                           int targetHour,
                           const QDateTime &now) const;
};
