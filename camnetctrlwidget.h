#ifndef CAMNETCTRLWIDGET_H
#define CAMNETCTRLWIDGET_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QSqlRecord>
#include "tabmodelalicenter.h"
#include "netsocketmodel.h"
#include <QTimer>
#include "caminfodb.h"
#include "recordsavemodel.h"
#include <QCloseEvent>
#include "singledeviceinfo.h"


QT_BEGIN_NAMESPACE
namespace Ui { class CamNetCtrlWidget; }
QT_END_NAMESPACE

class CamNetCtrlWidget : public QWidget
{
    Q_OBJECT

public:
    CamNetCtrlWidget(QWidget *parent = nullptr);
    ~CamNetCtrlWidget();
    //初始化主界面显示的数据库
    void initViewTmpdb(QString &dbname,QString linkname);
    //初始化用于显示的控件tableview
    void initTableViewModel();
    //初始化用于存储的数据库
    void initRecordSaveDB(QString &dbname,QString linkname);
    //重写了窗口关闭事件，使得关闭的时候回弹出对话框，确认是否关闭
    void closeEvent(QCloseEvent *event);
    //在程序退出的时候，将临时产生的文件夹下的所有东西都删除
    bool removeTmpDirRecursively(QString dirname);

private slots:
    //启动按钮
    void on_ButtonStartUp_clicked();
    //关闭按钮
    void on_ButtonShut_clicked();
    //清除列表按钮
    void on_ButtonClear_clicked();
    //双击tableview中的记录
    void on_tViewTmpdb_doubleClicked(const QModelIndex &index);
    //退出按钮
    void on_ButtonQuit_clicked();
    //获取了新记录
    void onGetNewMsg(QStringList datalist);
    //获取了新的回显信息
    //其实就是弹出一个对话框，给两个按钮，确认和取消
    void onGetEchoMsg(QString echomsg);

//    void onGetDeviceIDForSearch(QString deviceid);

signals:

private:
    Ui::CamNetCtrlWidget *ui;

//    QSqlDatabase *m_ViewTmpdb;

//    QSqlQuery *m_QueryTmpdb;

    TabModelAliCenter *m_TableViewModel;

    NetSocketModel *m_ServerModel;

    QTimer *m_RefreshTimer;

    QTimer *m_removelogTimer;
    CamInfoDB *m_CamInfoTmpdb;

    RecordSaveModel *m_RecordSaveDB;

    SingleDeviceInfo *m_singeldeviceinfo;

};
#endif // CAMNETCTRLWIDGET_H
