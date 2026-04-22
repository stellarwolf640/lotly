#include "lotreport.h"

#include <utility>

LotReport::LotReport(QString lotId, QString userId, Status status,
                     double trustScore, QDateTime timestamp)
    : m_lotId(std::move(lotId))
    , m_userId(std::move(userId))
    , m_status(status)
    , m_trustScore(trustScore)
    , m_timestamp(std::move(timestamp))
{
}

QString LotReport::lotId() const { return m_lotId; }
QString LotReport::userId() const { return m_userId; }
LotReport::Status LotReport::status() const { return m_status; }
double LotReport::trustScore() const { return m_trustScore; }
QDateTime LotReport::timestamp() const { return m_timestamp; }

QString LotReport::statusLabel() const
{
    return statusToString(m_status);
}

QString LotReport::statusToString(Status status)
{
    switch (status) {
    case Status::Empty:      return QStringLiteral("Empty");
    case Status::Moderate:   return QStringLiteral("Moderate");
    case Status::NearlyFull: return QStringLiteral("Nearly Full");
    case Status::Full:       return QStringLiteral("Full");
    }
    return QStringLiteral("Unknown");
}

LotReport::Status LotReport::statusFromString(const QString &str)
{
    if (str.compare(QStringLiteral("Empty"),      Qt::CaseInsensitive) == 0) return Status::Empty;
    if (str.compare(QStringLiteral("NearlyFull"), Qt::CaseInsensitive) == 0) return Status::NearlyFull;
    if (str.compare(QStringLiteral("Nearly Full"),Qt::CaseInsensitive) == 0) return Status::NearlyFull;
    if (str.compare(QStringLiteral("Full"),       Qt::CaseInsensitive) == 0) return Status::Full;
    return Status::Moderate;
}
