#include "dataprocessor.h"

double DataProcessor::reportSeverityAverage(const QList<LotReport> &reports) const
{
    if (reports.isEmpty())
        return 1.5;

    double total = 0.0;
    for (const LotReport &r : reports)
        total += static_cast<int>(r.status());

    return total / static_cast<double>(reports.size());
}

double DataProcessor::reportSeverityWeighted(const QList<LotReport> &reports) const
{
    if (reports.isEmpty())
        return 1.5;

    double weightedSum = 0.0;
    double totalWeight = 0.0;

    for (const LotReport &r : reports) {
        const double w = r.trustScore();
        weightedSum += static_cast<int>(r.status()) * w;
        totalWeight += w;
    }

    if (totalWeight <= 0.0)
        return reportSeverityAverage(reports);

    return weightedSum / totalWeight;
}

double DataProcessor::totalTrustWeight(const QList<LotReport> &reports) const
{
    double total = 0.0;
    for (const LotReport &r : reports)
        total += r.trustScore();
    return total;
}

int DataProcessor::latestReportCount(const QList<LotReport> &reports) const
{
    return reports.size();
}
