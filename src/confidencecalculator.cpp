#include "confidencecalculator.h"

#include <QtGlobal>

double ConfidenceCalculator::calculateConfidence(double totalTimeWeight) const
{
    return qMin(1.0, totalTimeWeight / 5.0);
}
