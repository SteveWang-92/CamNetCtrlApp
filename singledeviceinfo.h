#ifndef SINGLEDEVICEINFO_H
#define SINGLEDEVICEINFO_H

#include <QWidget>
#include <QUdpSocket>
namespace Ui {
class SingleDeviceInfo;
}

class SingleDeviceInfo : public QWidget
{
    Q_OBJECT

public:
    explicit SingleDeviceInfo(QWidget *parent = nullptr);
    ~SingleDeviceInfo();
    //在界面上显示数据
    void showCamData(QStringList *data);
    //检验配置的时间数值是不是合规的
    bool testTime();

    bool testLineEditTime();
    //检验电压的数值是不是在正常范围
    bool testVoltage();

    //void closeEvent(QCloseEvent *event);
signals:
    //给远端发送信息的信号
    void sendMsg(QString str, QHostAddress remoteadd, quint16 remoteport);
    //获取历史记录信号
    void getRecord(QString deviceid);

private slots:
    void on_Buttonmodify_clicked();

    void on_Buttonrestart_clicked();

    void on_Buttonclose_clicked();

    void on_Buttonback_clicked();

    void on_Buttonhistoryrecord_clicked();
private:
    //确认和取消对话框
    bool popupYN_Diaolog(QString str);
    //提示对话框
    void popupTipsDiaolog(QString str);

    Ui::SingleDeviceInfo *ui;
    QUdpSocket *clientCrl;

};

#endif // SINGLEDEVICEINFO_H
