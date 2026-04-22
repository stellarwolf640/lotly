#pragma once

#include <QList>

#include "lotreport.h"

class DataProcessor
{
public:
    // Simple unweighted average severity (0 = empty, 3 = full)
    double reportSeverityAverage(const QList<LotReport> &reports) const;

    // Trust-weighted severity — each report is weighted by its submitter's trust score.
    // This is the primary method used by PredictionEngine.
    double reportSeverityWeighted(const QList<LotReport> &reports) const;

    // Sum of trust scores across all reports (used as a data-sufficiency measure)
    double totalTrustWeight(const QList<LotReport> &reports) const;

    int latestReportCount(const QList<LotReport> &reports) const;
};
