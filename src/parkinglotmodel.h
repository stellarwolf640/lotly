#pragma once

#include <QAbstractListModel>

#include "parkinglot.h"

class ParkingLotModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles
    {
        IdRole = Qt::UserRole + 1,
        NameRole,
        LocationRole,
        TotalSpacesRole,
        PredictedAvailableSpacesRole,
        ConfidenceRole,
        ExplanationRole
    };
    Q_ENUM(Roles)

    explicit ParkingLotModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    void setLots(const QList<ParkingLot> &lots);
    ParkingLot lotAt(int index) const;
    ParkingLot lotById(const QString &lotId) const;

private:
    QList<ParkingLot> m_lots;
};
