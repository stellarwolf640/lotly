#pragma once

#include <QString>

class User
{
public:
    User() = default;

    // Legacy constructor — existing call-sites unchanged.
    User(QString id, QString displayName, double reliabilityScore = 0.75);

    // Full constructor with Google Auth fields.
    User(QString id, QString displayName, QString email,
         QString photoUrl, double reliabilityScore = 1.0);

    QString id()               const;
    QString displayName()      const;
    QString email()            const;
    QString photoUrl()         const;
    double  reliabilityScore() const;

    void setReliabilityScore(double score);
    void setEmail(const QString &email);
    void setPhotoUrl(const QString &photoUrl);

private:
    QString m_id;
    QString m_displayName;
    QString m_email;
    QString m_photoUrl;
    double  m_reliabilityScore = 0.75;
};
