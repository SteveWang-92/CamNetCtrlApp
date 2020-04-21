#ifndef TABMODELALICENTER_H
#define TABMODELALICENTER_H

#include <QObject>
#include <QSqlTableModel>
//主要是改写了QSqlTableModel的一个data方法，使得其可以居中显示，方便日后重用
//注意这里对构造函数进行了修改，增加了数据库参数的传递
class TabModelAliCenter : public QSqlTableModel
{
    Q_OBJECT
public:
    explicit TabModelAliCenter(QObject *parent = nullptr,QSqlDatabase db = QSqlDatabase());

    QVariant data(const QModelIndex &index, int role) const;
signals:

};

#endif // TABMODELALICENTER_H
