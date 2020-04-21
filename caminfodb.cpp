#include "caminfodb.h"
#include <QMessageBox>
#include <QSqlError>
#include <QPushButton>
#include <QDebug>
#include <QTime>
CamInfoDB::CamInfoDB(QObject *parent) : QObject(parent)
{
}

//初始化数据库，传入的参数一个是DB的名字，一个是链接的名字
void CamInfoDB::initInfoDB(QString dbname,QString linkname)
{
    m_database = new QSqlDatabase;
    *m_database = QSqlDatabase::addDatabase("QSQLITE",linkname);
    m_database->setDatabaseName(dbname);
    //如果数据库打不开的话，弹出一个对话框
    if (!m_database->open()){
        QMessageBox *echomsgbox = new QMessageBox;
        echomsgbox->setAttribute(Qt::WA_DeleteOnClose,true);
        echomsgbox->setIcon(QMessageBox::Warning);
        echomsgbox->setWindowTitle(QString::fromLocal8Bit("提示"));
        QPushButton *okbtn = new QPushButton(QString::fromLocal8Bit("确定"),echomsgbox);
        echomsgbox->addButton(okbtn,QMessageBox::AcceptRole);
        echomsgbox->setText(m_database->lastError().text());
        echomsgbox->show();
        echomsgbox->open();//阻塞等待用户输入
    }
    //链接数据库与query语句类
    m_dataquery = new QSqlQuery(*m_database);
    //query->exec("DROP TABLE TmpViewLog");        //先清空一下表
    //创建一个表，名字是TmpViewLog，里面的内容有ID,DEVICEID以及等等
    /*qDebug() <<*/ m_dataquery->exec("CREATE TABLE TmpViewLog ("
                    "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "DEVICEID VARCHAR(40) NOT NULL, "
                    "POSITION VARCHAR(40) NOT NULL, "
                    "STATE VARCHAR(40) NOT NULL, "
                    "VOLTAGE VARCHAR(40) NOT NULL,"
                    "SHUTVOLTAGE VARCHAR(40) NOT NULL, "
                    "REPOINTERVAL VARCHAR(40) NOT NULL, "
                    "STARTTIME VARCHAR(40) NOT NULL,"
                    "SHUTTIME VARCHAR(40) NOT NULL, "
                    "REPOTIME VARCHAR(40) NOT NULL, "
                    "IPADD VARCHAR(40) NOT NULL,"
//                    "PORT VARCHAR(40) NOT NULL)");
                    "PORT VARCHAR(40) NOT NULL,"
                    "[CreatedTime] TimeStamp NOT NULL DEFAULT (datetime('now','localtime')))");
}

QSqlDatabase* CamInfoDB::getDataBase()
{
    return m_database;
}

//传入的数据，并进行新增
bool CamInfoDB::addData(QStringList datalist)
{
    QString tmpforinsert;
    for (int i = 0; i < datalist.size(); ++i){
        tmpforinsert.append(QString("'%1',").arg(QString(datalist.at(i).toLocal8Bit())));
    }
    tmpforinsert.remove(tmpforinsert.size()-1,1);
//    qDebug() << tmpforinsert;
    return m_dataquery->exec(QString("INSERT INTO TmpViewLog "
                               "(DEVICEID, POSITION, STATE,VOLTAGE,"
                               "SHUTVOLTAGE, REPOINTERVAL, STARTTIME,"
                               "SHUTTIME, REPOTIME, IPADD, PORT) "
                                "VALUES (%1)")
                     .arg(tmpforinsert));
}

//修改数据或者说更新数据
bool CamInfoDB::modifyData(QStringList datalist)
{
    return m_dataquery->exec(QString("UPDATE TmpViewLog "
                            "SET POSITION = '%1', STATE = '%2', VOLTAGE = '%3',"
                            "SHUTVOLTAGE = '%4', REPOINTERVAL = '%5', STARTTIME = '%6',"
                            " SHUTTIME = '%7', REPOTIME = '%8', IPADD = '%9', PORT = '%10'"
                            " WHERE DEVICEID = '%11'")
                .arg(QString(datalist.at(1).toLocal8Bit()))
                .arg(QString(datalist.at(2).toLocal8Bit()))
                .arg(QString(datalist.at(3).toLocal8Bit()))
                .arg(QString(datalist.at(4).toLocal8Bit()))
                .arg(QString(datalist.at(5).toLocal8Bit()))
                .arg(QString(datalist.at(6).toLocal8Bit()))
                .arg(QString(datalist.at(7).toLocal8Bit()))
                .arg(QString(datalist.at(8).toLocal8Bit()))
                .arg(QString(datalist.at(9).toLocal8Bit()))
                .arg(QString(datalist.at(10).toLocal8Bit()))
                .arg(QString(datalist.at(0).toLocal8Bit())));
}

//判断是不是有这个记录，如果没有那么第一条是需要插入，后面的就是更新了
bool CamInfoDB::hasDeviceRecord(QString deviceid)
{
    m_dataquery->exec(QString("SELECT * FROM TmpViewLog WHERE DEVICEID = '%1'").arg(deviceid));
    //存在，那就更新
    //不存在，那就插入，
    return m_dataquery->next();
}

//调用自己的HistoryRecordView类进行展示特定deviceid的历史信息
void CamInfoDB::showhistory(QString deviceid)
{
    m_historyrecordview =new HistoryRecord;
    m_historyrecordview->setAttribute(Qt::WA_DeleteOnClose,true);
    m_historyrecordview->showHistoryRecord(deviceid,m_database);
    m_historyrecordview->show();
}

//超时的话那么需要设置其状态为离线
void CamInfoDB::onTimeOut()
{
    //由于超时功能指挥在显示用的数据库中才启用，所有直接选择所有的记录就行
    //因为所有的记录肯定是最新的
    m_dataquery->exec("SELECT ID,REPOTIME FROM TmpViewLog");
       QVector<int> timeoutID;
       while (m_dataquery->next()) {
           int ID = m_dataquery->value(0).toInt();
           //取出记录中的上报时间
           QTime start = QTime::fromString(m_dataquery->value(1).toString().split("-").at(1),"hh:mm");
           //与现有的时间进行对比
           QTime end = QTime::currentTime();
           //QTime的方法
           int interval = start.secsTo(end);
           if (interval > 900){
                timeoutID.append(ID);
           }
       }
       QVector<int>::iterator iter;
       for (iter=timeoutID.begin();iter!=timeoutID.end();iter++){
           m_dataquery->exec(QString("UPDATE TmpViewLog "
                                            "SET STATE = 'OFF-LINE' "
                                            "WHERE ID = %1").arg(*iter));
       }
}

bool CamInfoDB::clearAll()
{
        return m_dataquery->exec("DELETE FROM TmpViewLog");
}

bool CamInfoDB::clearTenDaysFormerLog()
{
    QString timeStamp =   QString(QDateTime::currentDateTime().addDays(-10).toString("yyyy-MM-dd hh:mm:ss"));
//    QString timeStamp =   QString(QDateTime::currentDateTime().addSecs(-10).toString("yyyy-MM-dd hh:mm:ss"));

    return m_dataquery->exec(QString("DELETE FROM TmpViewLog WHERE CreatedTime < '%1'")
                      .arg(timeStamp));
//    return m_dataquery->exec(QString("DELETE FROM TmpViewLog"));
}

void CamInfoDB::close()
{
    m_database->close();
}
