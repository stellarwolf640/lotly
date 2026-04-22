#include "parkinglot.h"

#include <utility>

ParkingLot::ParkingLot(QString id, QString name, QString location,
                       int totalSpaces, int predictedAvailableSpaces,
                       double confidence, QString explanation)
    : m_id(std::move(id))
    , m_name(std::move(name))
    , m_location(std::move(location))
    , m_totalSpaces(totalSpaces)
    , m_predictedAvailableSpaces(predictedAvailableSpaces)
    , m_confidence(confidence)
    , m_explanation(std::move(explanation))
{
}

ParkingLot::ParkingLot(QString id, QString name, QString location,
                       double latitude, double longitude, QString placeId,
                       int totalSpaces, int predictedAvailableSpaces,
                       double confidence, QString explanation)
    : m_id(std::move(id))
    , m_name(std::move(name))
    , m_location(std::move(location))
    , m_latitude(latitude)
    , m_longitude(longitude)
    , m_placeId(std::move(placeId))
    , m_totalSpaces(totalSpaces)
    , m_predictedAvailableSpaces(predictedAvailableSpaces)
    , m_confidence(confidence)
    , m_explanation(std::move(explanation))
{
}

QString ParkingLot::id() const { return m_id; }
QString ParkingLot::name() const { return m_name; }
QString ParkingLot::location() const { return m_location; }
QString ParkingLot::address() const { return m_address; }
double ParkingLot::latitude() const { return m_latitude; }
double ParkingLot::longitude() const { return m_longitude; }
QString ParkingLot::placeId() const { return m_placeId; }
int ParkingLot::totalSpaces() const { return m_totalSpaces; }
int ParkingLot::predictedAvailableSpaces() const { return m_predictedAvailableSpaces; }
double ParkingLot::confidence() const { return m_confidence; }
QString ParkingLot::explanation() const { return m_explanation; }
double ParkingLot::distanceMeters() const { return m_distanceMeters; }

void ParkingLot::setPredictedAvailableSpaces(int spaces) { m_predictedAvailableSpaces = spaces; }
void ParkingLot::setConfidence(double confidence) { m_confidence = confidence; }
void ParkingLot::setExplanation(QString explanation) { m_explanation = std::move(explanation); }
void ParkingLot::setTotalSpaces(int spaces) { m_totalSpaces = spaces; }
void ParkingLot::setAddress(const QString &address) { m_address = address; }
void ParkingLot::setDistanceMeters(double meters) { m_distanceMeters = meters; }
