#include "recordsavemodel.h"
#include <QDebug>
RecordSaveModel::RecordSaveModel(QObject *parent) : QObject(parent)
{
}

void RecordSaveModel::initRecordSaveModel(QString dbname,QString linkname)
{
    m_RecordSaveDB = new CamInfoDB;
    m_RecordSaveDB->initInfoDB(dbname,linkname);
}

bool RecordSaveModel::addNewRecord(QStringList recordlist)
{
    return m_RecordSaveDB->addData(recordlist);
}


void RecordSaveModel::slotHistoryRecord(QString deviceid)
{
    m_RecordSaveDB->showhistory(deviceid);
}

bool RecordSaveModel::clearTenDaysFormerLog(){
    return m_RecordSaveDB->clearTenDaysFormerLog();
}
