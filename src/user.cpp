#include "user.h"

#include <utility>

User::User(QString id, QString displayName, double reliabilityScore)
    : m_id(std::move(id))
    , m_displayName(std::move(displayName))
    , m_reliabilityScore(reliabilityScore)
{
}

User::User(QString id, QString displayName, QString email,
           QString photoUrl, double reliabilityScore)
    : m_id(std::move(id))
    , m_displayName(std::move(displayName))
    , m_email(std::move(email))
    , m_photoUrl(std::move(photoUrl))
    , m_reliabilityScore(reliabilityScore)
{
}

QString User::id()               const { return m_id; }
QString User::displayName()      const { return m_displayName; }
QString User::email()            const { return m_email; }
QString User::photoUrl()         const { return m_photoUrl; }
double  User::reliabilityScore() const { return m_reliabilityScore; }

void User::setReliabilityScore(double score) { m_reliabilityScore = score; }
void User::setEmail(const QString &email)    { m_email    = email; }
void User::setPhotoUrl(const QString &url)   { m_photoUrl = url; }

