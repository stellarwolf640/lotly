#include "user.h"

#include <utility>

User::User(QString id, QString displayName, double reliabilityScore)
    : m_id(std::move(id))
    , m_displayName(std::move(displayName))
    , m_reliabilityScore(reliabilityScore)
{
}

QString User::id() const
{
    return m_id;
}

QString User::displayName() const
{
    return m_displayName;
}

double User::reliabilityScore() const
{
    return m_reliabilityScore;
}

void User::setReliabilityScore(double score)
{
    m_reliabilityScore = score;
}
