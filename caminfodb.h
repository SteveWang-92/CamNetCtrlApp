#ifndef CAMINFODB_H
#define CAMINFODB_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include "historyrecord.h"

/*这是对数据库的一个封装
 * 实现了对数据库的初始化、增加记录、修改记录，
 * 超时设置状态为OFF-LINE、清除所有记录
 * 断开与数据库的连接
 * 还有就是展现历史数据
*/
class CamInfoDB : public QObject
{
    Q_OBJECT
public:
    explicit CamInfoDB(QObject *parent = nullptr);

    //初始化数据库
    void initInfoDB(QString dbname,QString linkname);
    //获取本地的数据库
    QSqlDatabase* getDataBase();
    //查询是否有记录
    bool hasDeviceRecord(QString deviceid);
    //增加数据
    bool addData(QStringList datalist);
    //修改数据，其实就是更新数据
    bool modifyData(QStringList datalist);
    //超时的话直接将数据库中的状态改为OFF-LINE
    void onTimeOut();
    //清除所有的记录
    bool clearAll();
    //清除十天之前的记录
    bool clearTenDaysFormerLog();
    //关闭与数据库的连接，用于后续的删除
    void close();
    //展示历史记录，调用了HistoryRecord类中的方法
    void showhistory(QString deviceid);

signals:

private:
    // 数据库
    QSqlDatabase *m_database;
    //操作数据库的QSqlQuery
    QSqlQuery *m_dataquery;
    //用于显示的HistoryRecord
    HistoryRecord *m_historyrecordview;

};

#endif // CAMINFODB_H
