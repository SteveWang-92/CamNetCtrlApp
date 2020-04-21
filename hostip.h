#ifndef HOSTIP_H
#define HOSTIP_H

#include <QObject>
//网上的一个获取本地IPv4地址的代码
class HostIP : public QObject
{
    Q_OBJECT
public:
    explicit HostIP(QObject *parent = nullptr);

    QString getLocalHostIP();
signals:

};

#endif // HOSTIP_H
