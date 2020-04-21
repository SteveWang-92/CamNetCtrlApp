#include "camnetctrlwidget.h"
#include "ui_camnetctrlwidget.h"
#include "hostip.h"
#include <QHostInfo>
#include <QDir>
#include <QDateTime>
#include <QMessageBox>
#include <QTextCodec>
#include <QScrollBar>


CamNetCtrlWidget::CamNetCtrlWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CamNetCtrlWidget)
{
    ui->setupUi(this);
    setWindowTitle("摄像头监控控制台");

    HostIP machineIP;
    //获取本机IP
    QString hostip = machineIP.getLocalHostIP();
    //获取主机名
    QString hostname = QHostInfo::localHostName();

    //显示上述信息
    ui->textHostName->setText(hostname);
    ui->textHostIP->setText(hostip);
    ui->textPort->setText(QString("8081"));
    ui->textHostName->setEnabled(false);
    ui->textHostIP->setEnabled(false);
    ui->textPort->setEnabled(false);
    ui->ButtonShut->setEnabled(false);

    QDir dir;
    //存储临时信息的文件夹
    if(!dir.exists("tmplog")){
        dir.mkdir("tmplog");
    }
    //存储历史信息的文件夹
    if(!dir.exists("log")){
        dir.mkdir("log");
    }
    //给临时和永久存储使用的数据库以名字和连接名，连接两个数据库，不给连接名会报错
    QString dirtmp_path="tmplog";
    QString tmpdbname = QString("./%1/TmpView.db").arg(dirtmp_path);
    QString tmplinkname = QString("tmplinksql");
    QString dirsave_path="log";
    QString savedbname = QString("./%1/CamReportInfo.db").arg(dirsave_path);
    QString savelinkname =  QString("savelinksql");
//    datetime=new QDateTime(QDateTime::currentDateTime());
//    QString date_now = datetime->toString("yyyy-MM-dd");

    //给予存储间隔的选项
    ui->comBoxTimeinterval->clear();
    QStringList strList;
    strList<<"请选择"<<"1"<<"5"<<"10"<<"20"<<"30";
    ui->comBoxTimeinterval->addItems(strList);

    //初始化显示用临时数据库
    initViewTmpdb(tmpdbname,tmplinkname);
    //初始化存储用数据库
    initRecordSaveDB(savedbname,savelinkname);
    //初始化显示模块
    initTableViewModel();
    //固定界面大小
    setFixedSize(this->width(),this->height());


}

CamNetCtrlWidget::~CamNetCtrlWidget()
{
    delete ui;
}

void CamNetCtrlWidget::initViewTmpdb(QString &tmpdbname,QString linkname)
{
    m_CamInfoTmpdb = new CamInfoDB(this);
    m_CamInfoTmpdb->initInfoDB(tmpdbname,linkname);
}


void CamNetCtrlWidget::initTableViewModel()
{
    //解决中文乱码的问题
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(codec);

//    m_TableViewModel = new TabModelAliCenter(this,*m_ViewTmpdb);
     m_TableViewModel = new TabModelAliCenter(this,*m_CamInfoTmpdb->getDataBase());
    m_TableViewModel->setTable("TmpViewLog");
    //以设备IP排序显示
    m_TableViewModel->setSort(1,Qt::AscendingOrder);
    //把Model放在tableView
    ui->tViewTmpdb->setModel(m_TableViewModel);
    //将ID列隐藏
    ui->tViewTmpdb->setColumnHidden(0,true);
    //时间戳一列
    ui->tViewTmpdb->setColumnHidden(12,true);
    //设置列，行表头的样式
    ui->tViewTmpdb->horizontalHeader()->setStyleSheet("QHeaderView::section {"
                                                "color: black;padding-left: 4px;border: 1px solid #6c6c6c;}");

    ui->tViewTmpdb->verticalHeader()->setStyleSheet("QHeaderView::section {"
                                                     "width:20px; color: black;padding-left: 4px;border: 1px solid #6c6c6c;}");
    //隐藏行表头
    ui->tViewTmpdb->verticalHeader()->hide();
    //设置tableViewdb和要显示的内容的宽度自适应，不会导致右边出现边框
    //ui->tViewTmpdb->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    //画网格线
    ui->tViewTmpdb->setGridStyle(Qt::SolidLine);
    ui->tViewTmpdb->setShowGrid(true);
    //选择模式
    ui->tViewTmpdb->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tViewTmpdb->setSelectionBehavior(QAbstractItemView::SelectRows);
    //设置垂直滚动条的宽度，并长期显示，主要目的是为了让界面的尺寸固定，美观
    ui->tViewTmpdb->verticalScrollBar()->setStyleSheet("QScrollBar:vertical {width: 20px;}");
    ui->tViewTmpdb->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    //设置列表头的名称，以及宽度
    QStringList HStrList;
         HStrList <<"序 号" << "设备ID号" << "设备位置" << "当前状态"
                  << "当前电压" <<"下限电压" << "上报间隔"<< "开启时间"
                  << "关闭时间"<<"上报时间"<<"IP地址"<<"端口号";
    int HlableCnt = HStrList.count();
    for (int i = 0; i < HlableCnt; i++) {
        m_TableViewModel->setHeaderData(i,Qt::Horizontal, HStrList.at(i));
        ui->tViewTmpdb->setColumnWidth(i,100);
    }
    ui->tViewTmpdb->setColumnWidth(9,150);
    ui->tViewTmpdb->setColumnWidth(10,150);
    ui->tViewTmpdb->resize(1220,800);
    //设置行高变成最小
    ui->tViewTmpdb->verticalHeader()->setDefaultSectionSize(0);
    ui->tViewTmpdb->verticalHeader()->setMinimumSectionSize(0);
    //设置手动提交才生效
    m_TableViewModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    //显示model的数据
    m_TableViewModel->select();
}

void CamNetCtrlWidget::on_ButtonStartUp_clicked()
{
    //获取主界面的时间信息
    if(ui->comBoxTimeinterval->currentIndex()==0){
        onGetEchoMsg("请选择数据存储时间间隔/分钟");
        return;
    }
    //将这个控件不使能
    ui->comBoxTimeinterval->setEnabled(false);
    //打开一个UDP套接字，绑定到8081端口，不允许其他进程进行端口的绑定
    m_ServerModel = new NetSocketModel(this,QHostAddress(QString(ui->textHostIP->text())),ui->textPort->text().toUInt());

    ui->ButtonShut->setEnabled(true);
    ui->ButtonStartUp->setEnabled(false);
    //使用一个计时器，20秒对界面中的状态信息检查一次，超时就设置为OFF-LINE
    m_RefreshTimer = new QTimer(this);
    if(m_RefreshTimer->isActive() == false){
        m_RefreshTimer->start(20000);
    }
    //计时器，一分钟更新一次设备状态
    connect(m_RefreshTimer, &QTimer::timeout,
            [=](){
               m_CamInfoTmpdb->onTimeOut();
               m_TableViewModel->select();
               }
            );

    m_removelogTimer= new QTimer(this);
    if(m_removelogTimer->isActive() == false){
        m_removelogTimer->start(3600000);
//                m_removelogTimer->start(3000);
    }
    //计时器，每一小时将10天前的设备信息删除
    connect(m_removelogTimer, &QTimer::timeout,
            [=](){
               m_RecordSaveDB->clearTenDaysFormerLog();
               }
            );

    //网络模块收到信息，交给本界面的槽函数处理
    connect(m_ServerModel,&NetSocketModel::getNewRecord,this,&CamNetCtrlWidget::onGetNewMsg);

    connect(m_ServerModel,&NetSocketModel::echoRemoteMsg,this,&CamNetCtrlWidget::onGetEchoMsg);
}


void CamNetCtrlWidget::on_ButtonShut_clicked()
{
    //关闭服务器，将时间间隔的控件使能
    m_ServerModel->shutSocket();
    ui->ButtonShut->setEnabled(false);
    ui->ButtonStartUp->setEnabled(true);
    ui->comBoxTimeinterval->setEnabled(true);
    if(m_RefreshTimer->isActive() == true){
        m_RefreshTimer->stop();
    }

}

void CamNetCtrlWidget::on_ButtonClear_clicked()
{
//    m_QueryTmpdb->exec("DELETE FROM TmpViewLog");
    m_CamInfoTmpdb->clearAll();
    m_TableViewModel->select();
    qDebug() << m_RecordSaveDB->clearTenDaysFormerLog();
}

void CamNetCtrlWidget::on_tViewTmpdb_doubleClicked(const QModelIndex &index)
{

    QAbstractItemModel *Imodel=ui->tViewTmpdb->model();

    QStringList datalist;
    for (int i = 0; i < Imodel->columnCount(); i++) {
        //index.row()为算选择的行号。1为所选中行的第一列。。
        QModelIndex Iindex = Imodel->index(index.row(),i);
        QVariant datatmp=Imodel->data(Iindex);
        datalist.append(datatmp.toString());
    }
//打开单节点信息界面
    m_singeldeviceinfo = new SingleDeviceInfo;
    m_singeldeviceinfo->setAttribute(Qt::WA_DeleteOnClose,true);
    m_singeldeviceinfo->setWindowTitle("节点信息");
    m_singeldeviceinfo->show();
    m_singeldeviceinfo->showCamData(&datalist);
//将此界面的发送消息的信号和网络发送信息的槽函数连接起来
    connect(m_singeldeviceinfo,&SingleDeviceInfo::sendMsg,m_ServerModel,&NetSocketModel::sendRemoteMsg);
    //获取设备历史信息
    connect(m_singeldeviceinfo,&SingleDeviceInfo::getRecord,m_RecordSaveDB,&RecordSaveModel::slotHistoryRecord);
}

void CamNetCtrlWidget::on_ButtonQuit_clicked()
{
    this->close();
}

void CamNetCtrlWidget::onGetNewMsg(QStringList datalist)
{
    QString deviceid = QString(datalist.at(0).toLocal8Bit());
    //存在，那就更新
    //不存在，那就插入，
    bool isExist = m_CamInfoTmpdb->hasDeviceRecord(deviceid);
    if(!isExist){
        m_CamInfoTmpdb->addData(datalist);
      }
    else {
        m_CamInfoTmpdb->modifyData(datalist);
    }
    //实现了按照窗口上的时间间隔来进行存储数据
    int saveinterval =ui->comBoxTimeinterval->currentText().toInt();
    //获取上报的分钟数
    int tmpminnum = datalist.at(8).right(2).toInt();
    if(tmpminnum%saveinterval == 0){
    m_RecordSaveDB->addNewRecord(datalist);
    }
    m_TableViewModel->select();
}

void CamNetCtrlWidget::onGetEchoMsg(QString echomsg)
{
    //显示一个提示框
    QMessageBox *echomsgbox = new QMessageBox;
    echomsgbox->setAttribute(Qt::WA_DeleteOnClose,true);
    echomsgbox->setIcon(QMessageBox::Warning);
    echomsgbox->setWindowTitle(QString::fromLocal8Bit("提示"));
    QPushButton *okbtn = new QPushButton(QString::fromLocal8Bit("确定"),echomsgbox);
    echomsgbox->addButton(okbtn,QMessageBox::AcceptRole);
    echomsgbox->setText(echomsg);
    echomsgbox->show();
    //echomsgbox->open();//阻塞等待用户输入
}

void CamNetCtrlWidget::initRecordSaveDB(QString &dbname,QString linkname)
{
    m_RecordSaveDB =new RecordSaveModel(this);
    m_RecordSaveDB->initRecordSaveModel(dbname,linkname);
}


void CamNetCtrlWidget::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton button;
    button=QMessageBox::question(this,tr("退出程序"),QString(tr("确认退出程序")),QMessageBox::Yes|QMessageBox::No);
    if(button==QMessageBox::No)
    {
        event->ignore(); // 忽略退出信号，程序继续进行
    }
    else if(button==QMessageBox::Yes)
    {
        this->removeTmpDirRecursively(QString("tmplog"));
        event->accept(); // 接受退出信号，程序退出
    }
}

bool CamNetCtrlWidget::removeTmpDirRecursively(QString dirname)
{
        //保证db断开连接
        m_CamInfoTmpdb->close();
        QDir dir(dirname);
        QFileInfoList fileList;
        QFileInfo curFile;
        if(!dir.exists())  {return false;}//文件不存，则返回false
        fileList=dir.entryInfoList(QDir::Dirs|QDir::Files
                                   |QDir::Readable|QDir::Writable
                                   |QDir::Hidden|QDir::NoDotAndDotDot
                                   ,QDir::Name);
        while(fileList.size()>0){
            int infoNum=fileList.size();
            for(int i=infoNum-1;i>=0;i--){
                curFile=fileList[i];
                //如果是文件，删除文件
                if(curFile.isFile()){
                    QFile fileTemp(curFile.filePath());
                    fileTemp.remove();
                    fileList.removeAt(i);
                }
                //如果是文件夹
                if(curFile.isDir()){
                    QDir dirTemp(curFile.filePath());
                    QFileInfoList fileList1=dirTemp.entryInfoList(QDir::Dirs|QDir::Files
                                                                  |QDir::Readable|QDir::Writable
                                                                  |QDir::Hidden|QDir::NoDotAndDotDot
                                                                  ,QDir::Name);
                    //下层没有文件或文件夹
                    if(fileList1.size()==0){
                        dirTemp.rmdir(".");
                        fileList.removeAt(i);
                    }
                    //下层有文件夹或文件
                    else{
                        for(int j=0;j<fileList1.size();j++){
                            if(!(fileList.contains(fileList1[j])))
                                fileList.append(fileList1[j]);
                        }
                        }
                    }
                }
            }
        dir.setPath("./");
        dir.rmdir(dirname);
        return true;
}


