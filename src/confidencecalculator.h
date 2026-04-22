#pragma once

#include <QList>

class LotReport;

class ConfidenceCalculator
{
public:
    double calculateConfidence(const QList<LotReport> &reports) const;
};
