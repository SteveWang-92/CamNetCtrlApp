#include "tabmodelalicenter.h"

TabModelAliCenter::TabModelAliCenter(QObject *parent, QSqlDatabase db) : QSqlTableModel(parent,db)
{

}

QVariant TabModelAliCenter::data(const QModelIndex &item, int role) const

{
    QVariant value = QSqlQueryModel::data(item, role);
    if(role ==  Qt::TextAlignmentRole )
    {
        value   =   (Qt::AlignCenter);
        return value;
    }
    return value;
}
