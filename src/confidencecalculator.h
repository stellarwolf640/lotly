#pragma once

// ConfidenceCalculator converts a time-weighted sum of reports into a
// confidence score in [0, 1].
//
// Formula: min(1.0, totalTimeWeight / 5.0)
//   totalTimeWeight 0   → 0.00 (no data — purely fallback curve)
//   totalTimeWeight 1   → 0.20
//   totalTimeWeight 2.5 → 0.50
//   totalTimeWeight 5+  → 1.00 (saturated — very high data coverage)
//
// The input is produced by DataProcessor::totalTimeWeight(), which already
// incorporates age decay, hour proximity, and per-user trust scores.
class ConfidenceCalculator
{
public:
    // Returns a confidence value in [0, 1].
    // Pass DataProcessor::totalTimeWeight(reports, hour, now) as the argument.
    double calculateConfidence(double totalTimeWeight) const;
};
