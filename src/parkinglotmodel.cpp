#include "parkinglotmodel.h"

ParkingLotModel::ParkingLotModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int ParkingLotModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return m_lots.size();
}

QVariant ParkingLotModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_lots.size()) {
        return {};
    }

    const ParkingLot &lot = m_lots.at(index.row());
    switch (role) {
    case IdRole:
        return lot.id();
    case NameRole:
        return lot.name();
    case LocationRole:
        return lot.location();
    case TotalSpacesRole:
        return lot.totalSpaces();
    case PredictedAvailableSpacesRole:
        return lot.predictedAvailableSpaces();
    case ConfidenceRole:
        return lot.confidence();
    case ExplanationRole:
        return lot.explanation();
    default:
        return {};
    }
}

QHash<int, QByteArray> ParkingLotModel::roleNames() const
{
    return {
        {IdRole, "lotId"},
        {NameRole, "name"},
        {LocationRole, "location"},
        {TotalSpacesRole, "totalSpaces"},
        {PredictedAvailableSpacesRole, "predictedAvailableSpaces"},
        {ConfidenceRole, "confidence"},
        {ExplanationRole, "explanation"},
    };
}

void ParkingLotModel::setLots(const QList<ParkingLot> &lots)
{
    beginResetModel();
    m_lots = lots;
    endResetModel();
}

ParkingLot ParkingLotModel::lotAt(int index) const
{
    if (index < 0 || index >= m_lots.size()) {
        return {};
    }

    return m_lots.at(index);
}

ParkingLot ParkingLotModel::lotById(const QString &lotId) const
{
    for (const ParkingLot &lot : m_lots) {
        if (lot.id() == lotId) {
            return lot;
        }
    }

    return {};
}
