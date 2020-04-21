#ifndef RECORDSAVEMODEL_H
#define RECORDSAVEMODEL_H

#include <QSharedData>
#include "caminfodb.h"

//这个类主要是对CamInfoDB的一个封装，个人感觉是多余的，
//主要是将名字进行了重新命名，为了方便和显示用的数据库进行区分
class RecordSaveModel : public QObject
{
    Q_OBJECT
public:
    explicit RecordSaveModel(QObject *parent = nullptr);

    void initRecordSaveModel(QString dbname,QString linkname);

    bool addNewRecord(QStringList recordlist);

    bool clearTenDaysFormerLog();

public slots:

    void slotHistoryRecord(QString deviceid);


private:
    CamInfoDB *m_RecordSaveDB;

};

#endif // RECORDSAVEMODEL_H
