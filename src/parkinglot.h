#pragma once

#include <QString>

class ParkingLot
{
public:
    ParkingLot() = default;

    // Legacy constructor — no geo data (used by seed code)
    ParkingLot(QString id, QString name, QString location,
               int totalSpaces, int predictedAvailableSpaces,
               double confidence, QString explanation);

    // Full constructor with geo coordinates and place ID
    ParkingLot(QString id, QString name, QString location,
               double latitude, double longitude, QString placeId,
               int totalSpaces, int predictedAvailableSpaces,
               double confidence, QString explanation);

    QString id() const;
    QString name() const;
    QString location() const;
    QString address() const;
    double latitude() const;
    double longitude() const;
    QString placeId() const;
    int totalSpaces() const;
    int predictedAvailableSpaces() const;
    double confidence() const;
    QString explanation() const;
    double distanceMeters() const;  // -1 if unknown

    void setPredictedAvailableSpaces(int spaces);
    void setConfidence(double confidence);
    void setExplanation(QString explanation);
    void setTotalSpaces(int spaces);
    void setAddress(const QString &address);
    void setDistanceMeters(double meters);

private:
    QString m_id;
    QString m_name;
    QString m_location;
    QString m_address;
    double m_latitude = 0.0;
    double m_longitude = 0.0;
    QString m_placeId;
    int m_totalSpaces = 0;
    int m_predictedAvailableSpaces = 0;
    double m_confidence = 0.0;
    QString m_explanation;
    double m_distanceMeters = -1.0;
};
