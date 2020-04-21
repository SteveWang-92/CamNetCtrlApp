
#include "hostip.h"
#include <QHostAddress>
#include <QNetworkInterface>
HostIP::HostIP(QObject *parent) : QObject(parent)
{

}

/*
 * 名称:get_localmachine_ip
 * 功能:获取本机的IP地址
 * 参数:no
 * 返回:QString
 */
QString HostIP::getLocalHostIP()
{
    QString ipAddress;
    //获取所有的网络接口
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    // use the first non-localhost IPv4 address
    for (int i = 0; i < ipAddressesList.size(); ++i) {
        //判断不等于环回地址127.0.0.1，并且转换IPv4地址可行，那么就获取这个地址最为返回值
        if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
            ipAddressesList.at(i).toIPv4Address()) {
            ipAddress = ipAddressesList.at(i).toString();
            break;
        }
    }
    // if we did not find one, use IPv4 localhost
    //如果没有的话，那么就使用环回地址
    if (ipAddress.isEmpty())
        ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
    return ipAddress;
}
