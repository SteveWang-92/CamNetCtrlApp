#include "netsocketmodel.h"
#include <QDateTime>
NetSocketModel::NetSocketModel(QObject *parent,QHostAddress hostip,quint16 port) : QObject(parent)
{
    //构造的时候要知道需要使用的IP以及接口
    m_udpsocket = new QUdpSocket;
    m_udpsocket->bind(QHostAddress(hostip),port,
                     QAbstractSocket::ShareAddress);

    //有数据来了，使用这个函数进行处理
    connect(m_udpsocket , &QUdpSocket::readyRead ,
            [=]() mutable
            {   //设置，buff用于取出数据，以及对端的ＩＰ，端口，以及此时的时间
                char buff[1024] = "";
                QHostAddress clientadd;
                quint16 clientport;
                //将ＵＤＰ数据读取至ｂｕｆｆ中
                qint64 len =  m_udpsocket->readDatagram(buff , sizeof (buff) , &clientadd ,&clientport);
                QString checkInfo(buff);
                QStringList checkInfolist = checkInfo.split("-");
                //如果有Config字符那么发送回显信号
                if( checkInfo.contains("Config",Qt::CaseSensitive) )
                {
                    emit echoRemoteMsg(QString("配置成功！"));
                    len = 0;
                }
                else if (checkInfo.contains("Reset",Qt::CaseSensitive)) {
                    emit echoRemoteMsg(QString("重启成功！"));
                    len = 0;
                }
                else if (checkInfo.contains("ShutDown",Qt::CaseSensitive)) {
                    emit echoRemoteMsg(QString("关闭成功！"));
                    len = 0;
                }
                //如果不是想应的信息，就直接丢弃
                else if (checkInfolist.count()!= 9 || !checkInfo.endsWith('/')) {
                    len = 0;
                }
                //判断，如果读取成功，就对需要的信息进行重新的组装，发送获取了新纪录的信号
                if (len >0)
                {
                    QString content = QString(buff);
                    content.remove(content.size()-1,1);
                    QStringList contentlist_tmp = content.split("/");
                    QStringList  contentlist = contentlist_tmp.at(0).split("-");
                    contentlist.append(contentlist_tmp.at(1));
                    contentlist.append(QString("%1").arg(clientadd.toString()));
                    contentlist.append(QString("%1").arg(clientport));
                    emit getNewRecord(contentlist);
                }
    }
    );
}

void NetSocketModel::sendRemoteMsg(QString msg,QHostAddress remoteIP, quint16 remotePort)
{
     m_udpsocket->writeDatagram(msg.toLocal8Bit().data(),msg.size(),remoteIP,remotePort);
}

void NetSocketModel::shutSocket()
{
    m_udpsocket->disconnectFromHost();
}
