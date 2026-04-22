#pragma once

#include <QString>

class User
{
public:
    User() = default;
    User(QString id, QString displayName, double reliabilityScore = 0.75);

    QString id() const;
    QString displayName() const;
    double reliabilityScore() const;

    void setReliabilityScore(double score);

private:
    QString m_id;
    QString m_displayName;
    double m_reliabilityScore = 0.75;
};
