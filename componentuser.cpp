#include "componentuser.h"

componentUser::componentUser(QObject *parent)
    : QAbstractItemModel(parent)
{}

QVariant componentUser::headerData(int section, Qt::Orientation orientation, int role) const
{
    // FIXME: Implement me!
}

bool componentUser::setHeaderData(int section,
                                  Qt::Orientation orientation,
                                  const QVariant &value,
                                  int role)
{
    if (value != headerData(section, orientation, role)) {
        // FIXME: Implement me!
        emit headerDataChanged(orientation, section, section);
        return true;
    }
    return false;
}

QModelIndex componentUser::index(int row, int column, const QModelIndex &parent) const
{
    // FIXME: Implement me!
}

QModelIndex componentUser::parent(const QModelIndex &index) const
{
    // FIXME: Implement me!
}

int componentUser::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return 0;

    // FIXME: Implement me!
}

int componentUser::columnCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return 0;

    // FIXME: Implement me!
}

QVariant componentUser::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    // FIXME: Implement me!
    return QVariant();
}

bool componentUser::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (data(index, role) != value) {
        // FIXME: Implement me!
        emit dataChanged(index, index, {role});
        return true;
    }
    return false;
}

Qt::ItemFlags componentUser::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable; // FIXME: Implement me!
}
