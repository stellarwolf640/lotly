#include "confidencecalculator.h"

#include <QtGlobal>

#include "lotreport.h"

double ConfidenceCalculator::calculateConfidence(const QList<LotReport> &reports) const
{
    if (reports.isEmpty())
        return 0.35;  // Fallback-only baseline

    // Sum trust scores — a single highly-trusted user counts more than several noisy ones
    double totalTrust = 0.0;
    for (const LotReport &r : reports)
        totalTrust += r.trustScore();

    // Each 1.0 of effective trust adds ~8% confidence; capped at 97%
    const double confidence = 0.35 + (totalTrust * 0.08);
    return qMin(0.97, confidence);
}
