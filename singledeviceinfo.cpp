#include "singledeviceinfo.h"
#include "ui_singledeviceinfo.h"
#include <QtDebug>
#include <QMessageBox>
#include <QHostAddress>
#include "hostip.h"
#include <QNetworkDatagram>
#include <QTime>
#include <QCloseEvent>

SingleDeviceInfo::SingleDeviceInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SingleDeviceInfo)
{
    ui->setupUi(this);
    //设置为模态窗口，只能操作当前窗口
    setAttribute(Qt::WA_ShowModal,true);
    //setAttribute(Qt::WA_DeleteOnClose,true);
}

SingleDeviceInfo::~SingleDeviceInfo()
{
    delete ui;
}

void SingleDeviceInfo::showCamData(QStringList *data)
{
    ui->lineEditID->setText(data->at(1));
    ui->lineEditIP->setText(data->at(10));
    ui->lineEditPORT->setText(data->at(11));
    ui->lineEditID->setEnabled(false);
    ui->lineEditIP->setEnabled(false);
    ui->lineEditPORT->setEnabled(false);
//    ui->lineEditID->setFocusPolicy(Qt::NoFocus);
//    ui->lineEditIP->setFocusPolicy(Qt::NoFocus);
//    ui->lineEditPORT->setFocusPolicy(Qt::NoFocus);
    QString inv = data->at(6);
    inv.remove("min",Qt::CaseInsensitive);
    ui->lineEditinterval->setText(inv);
    ui->lineEditclosetime->setText(data->at(8));
    ui->lineEditstarttime->setText(data->at(7));
    QString val = data->at(5);
    val.remove("V",Qt::CaseInsensitive);
    ui->lineEditlowvoltage->setText(val);
    ui->lineEdittmpclosetime->setText("0000");
    ui->lineEdittmpstarttime->setText("0000");
    setFixedSize(this->width(),this->height());
}

//修改信息点击了之后就给通信模块发送信息
void SingleDeviceInfo::on_Buttonmodify_clicked()
{
    if (testLineEditTime()&&testVoltage()){
        bool isSure = this->popupYN_Diaolog(QString("确定更改配置吗？"));
        if (isSure)
        {
            QString modifymsg = QString("%1-%2-%3-%4-%5-%6-%7/")
                    .arg(ui->lineEditID->text())
                    .arg(ui->lineEditinterval->text().append("min"))
                    .arg(ui->lineEditlowvoltage->text().append('V'))
                    .arg(ui->lineEditstarttime->text())
                    .arg(ui->lineEditclosetime->text())
                    .arg(ui->lineEdittmpclosetime->text())
                    .arg(ui->lineEdittmpstarttime->text());
            QHostAddress clientIp = QHostAddress(ui->lineEditIP->text());
            quint16 clientPort =  ui->lineEditPORT->text().toInt();
            emit sendMsg(modifymsg,clientIp,clientPort);
        }
        else{
        }
    }
}

void SingleDeviceInfo::on_Buttonrestart_clicked()
{
    bool isSure = this->popupYN_Diaolog(QString("确定重新启动吗？"));
    if (isSure)
    {
        QHostAddress clientIp = QHostAddress(ui->lineEditIP->text());
        quint16 clientPort =  ui->lineEditPORT->text().toInt();
        emit sendMsg(QString("%1-Reset/").arg(ui->lineEditID->text()),clientIp,clientPort);
//        close();
    }
    else{
    }
}

void SingleDeviceInfo::on_Buttonclose_clicked()
{
    bool isSure = this->popupYN_Diaolog(QString("确定关闭吗？"));
    if (isSure)
    {
        QHostAddress clientIp = QHostAddress(ui->lineEditIP->text());
        quint16 clientPort =  ui->lineEditPORT->text().toInt();
        emit sendMsg(QString("%1-ShutDown/").arg(ui->lineEditID->text()),clientIp,clientPort);
//        close();
    }
    else{
    }
}

void SingleDeviceInfo::on_Buttonback_clicked()
{
    this->close();
}

bool SingleDeviceInfo::testTime()
{
    QStringList timelist;
    timelist.append(ui->lineEditclosetime->text());
    timelist.append(ui->lineEditstarttime->text());
    timelist.append(ui->lineEdittmpclosetime->text());
    timelist.append(ui->lineEdittmpstarttime->text());
    QTime ttimelist[timelist.size()];
    for (int i = 0; i < timelist.size(); ++i){
        QTime Time =   QTime::fromString(timelist[i], "hhmm");
        if (Time.isValid()){
            ttimelist[i] = Time;
        }
        else{
            return false;
        }
    }

    if(ttimelist[0] > ttimelist[1]){
        return true;
    }
    else {
        return false;
    }
}

bool SingleDeviceInfo::testLineEditTime()
{
            if(!testTime()) {
                 this->popupTipsDiaolog(QString("请输入正确的时间! (范围：0000-2359)"));
                 return false;
            }
            else {
                return true;
            }
}

bool SingleDeviceInfo::testVoltage()
{
    bool ok;
    int vol =   ui->lineEditlowvoltage->text().toInt(&ok,10);
    if(ok){
       if(vol>=9 && vol <= 13)
           return true;
    }
    this->popupTipsDiaolog(QString("请输入正确电压! (范围:9-13)"));
    return false;
}

void SingleDeviceInfo::on_Buttonhistoryrecord_clicked()
{
    emit getRecord(ui->lineEditID->text());
}

//void SingleDeviceInfo::closeEvent(QCloseEvent *event)
//{
//    QMessageBox::StandardButton button;
//    button=QMessageBox::question(this,tr("退出窗口"),QString(tr("确认退出本窗口")),QMessageBox::Yes|QMessageBox::No);
//    if(button==QMessageBox::No){
//        event->ignore(); // 忽略退出信号，程序继续进行
//    }
//    else if(button==QMessageBox::Yes){
//        event->accept(); // 接受退出信号，程序退出
//    }
//}
bool SingleDeviceInfo::popupYN_Diaolog(QString str)
{
    QPushButton *okbtn = new QPushButton(QString::fromLocal8Bit("确定"),this);
    QPushButton *cancelbtn = new QPushButton(QString::fromLocal8Bit("取消"),this);
    QMessageBox *modifymsgbox = new QMessageBox(this);
    modifymsgbox->setAttribute(Qt::WA_DeleteOnClose,true);
    modifymsgbox->setIcon(QMessageBox::Warning);
    modifymsgbox->setWindowTitle(QString::fromLocal8Bit("提示"));
    modifymsgbox->setText(str);
    modifymsgbox->addButton(okbtn, QMessageBox::AcceptRole);
    modifymsgbox->addButton(cancelbtn, QMessageBox::RejectRole);
    modifymsgbox->show();
    modifymsgbox->exec();//阻塞等待用户输入
    if (modifymsgbox->clickedButton()==okbtn){
        return true;
    }
    else{
        return false;
    }
}
void SingleDeviceInfo::popupTipsDiaolog(QString str)
{
    QMessageBox *msgbox = new QMessageBox(this);
    msgbox->setIcon(QMessageBox::Warning);
    msgbox->setWindowTitle(QString::fromLocal8Bit("提示"));
    QPushButton *okbtn = new QPushButton(QString::fromLocal8Bit("确定"),this);
    msgbox->setText(str);
    msgbox->addButton(okbtn,QMessageBox::AcceptRole);
    msgbox->show();
    msgbox->open();//阻塞等待用户输入
}
