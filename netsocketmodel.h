#ifndef NETSOCKETMODEL_H
#define NETSOCKETMODEL_H

#include <QObject>
#include <QUdpSocket>
#include <QStringList>
//套接字模块
class NetSocketModel : public QObject
{
    Q_OBJECT
public:
    //构造的时候就需要IP地址和端口
    explicit NetSocketModel(QObject *parent = nullptr, QHostAddress hostip = QHostAddress(),quint16 port = 8081);
    //关闭套接字
    void shutSocket();

signals:
    //对应远端的回复信息的信号
    void echoRemoteMsg(QString msg);
    //对应获取新的记录的信号
    void getNewRecord(QStringList record);


public slots:
    //槽函数，给远端的设备发送信息
    void sendRemoteMsg(QString msg,QHostAddress remoteIP, quint16 remotePort);

private:
    QUdpSocket *m_udpsocket;
};

#endif // NETSOCKETMODEL_H
