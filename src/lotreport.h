#pragma once

#include <QDateTime>
#include <QString>

class LotReport
{
public:
    enum class Status
    {
        Empty = 0,
        Moderate,
        NearlyFull,
        Full
    };

    LotReport() = default;
    LotReport(QString lotId,
              QString userId,
              Status status,
              double trustScore = 0.75,
              QDateTime timestamp = QDateTime::currentDateTime());

    QString lotId() const;
    QString userId() const;
    Status status() const;
    double trustScore() const;
    QDateTime timestamp() const;
    QString statusLabel() const;

    static QString statusToString(Status status);
    static Status statusFromString(const QString &str);

private:
    QString m_lotId;
    QString m_userId;
    Status m_status = Status::Moderate;
    double m_trustScore = 0.75;
    QDateTime m_timestamp;
};
