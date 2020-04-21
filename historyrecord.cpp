#include "historyrecord.h"
#include "ui_historyrecord.h"
#include <QTextCodec>
#include <QDebug>
#include <QScrollBar>
#include <QDateTime>
#include <QDateTimeAxis>
#include <QSqlRecord>
#include <QSplineSeries>
HistoryRecord::HistoryRecord(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HistoryRecord)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_ShowModal,true);
    setWindowTitle("设备历史记录");
    //setFixedSize(this->size());
    //setAttribute(Qt::WA_DeleteOnClose, true );
}

HistoryRecord::~HistoryRecord()
{
    delete ui;
}

void HistoryRecord::showHistoryRecord(QString Deviceid ,QSqlDatabase *db)
{
    m_db = new QSqlDatabase(*db);
    m_query = new QSqlQuery(*m_db);
    //解决中文乱码的问题
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(codec);

    m_model = new TabModelAliCenter(this, *m_db);
    m_model->setTable("TmpViewLog");

//    m_model->select();//显示model的数据


    m_model->setFilter(QString("DEVICEID = '%1'").arg(Deviceid));
    m_model->select();//显示model的数据

    ui->tableViewhistory->setModel(m_model);//把ｍｏｄｅｌ放在tableView
    ui->tableViewhistory->setColumnHidden(0,true);
    //时间戳一列
    ui->tableViewhistory->setColumnHidden(12,true);
    ui->tableViewhistory->horizontalHeader()->setStyleSheet("QHeaderView::section {"
                                                "color: black;padding-left: 4px;border: 1px solid #6c6c6c;}");
    ui->tableViewhistory->verticalHeader()->setStyleSheet("QHeaderView::section {"
                                                     "width:20px; color: black;padding-left: 4px;border: 1px solid #6c6c6c;}");
    ui->tableViewhistory->verticalHeader()->hide();

    ui->tableViewhistory->setGridStyle(Qt::SolidLine);
    ui->tableViewhistory->setShowGrid(true);
    ui->tableViewhistory->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableViewhistory->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableViewhistory->verticalScrollBar()->setStyleSheet("QScrollBar:vertical {width: 20px;}");
    ui->tableViewhistory->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    QStringList HStrList;
    HStrList<<"序 号" << "设备ID号" << "设备位置" << "当前状态"
           << "当前电压" <<"下限电压" << "上报间隔"<< "开启时间"
           << "关闭时间"<<"上报时间"<<"IP地址"<<"端口号";
//    ui->tableViewhistory->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
//        ui->tableViewdb->resizeRowsToContents();
//        ui->tableViewdb->resizeColumnsToContents();

    int HlableCnt = HStrList.count();
    for (int i = 0; i < HlableCnt; i++) {
        m_model->setHeaderData(i,Qt::Horizontal, HStrList.at(i));
        //所有列设置成80宽
        ui->tableViewhistory->setColumnWidth(i,100);
    }
    //IP地址以及时间列列设置成120宽
    ui->tableViewhistory->setColumnWidth(9,150);
    ui->tableViewhistory->setColumnWidth(10,150);
    //整个窗口的宽度应该是
//   列宽*总数-（第一列）-2*原有宽（9,10列）+2*现有宽（9,10列）+（verticalHeader宽度）
    ui->tableViewhistory->resize(1220,300);

    //设置手动提交才生效
    ui->tableViewhistory->verticalHeader()->setDefaultSectionSize(0);
    ui->tableViewhistory->verticalHeader()->setMinimumSectionSize(0);
    //设置选中的样式，并且设置初始选中的是最新的行
    ui->tableViewhistory->setStyleSheet(QStringLiteral
    //RGB颜色的数值可以通过电脑上的画图软件获得
                                         ("selection-background-color: rgb(20,192,235);"));


     m_query->exec(QString("SELECT * FROM TmpViewLog WHERE DEVICEID = '%1'").arg(Deviceid));
 //    m_query->exec(QString("SELECT * FROM TmpViewLog ORDER BY ID "));

     QLineSeries *series = new QLineSeries(this);
     QLineSeries *solidline  = new QLineSeries(this);
     QStringList linename;
     linename.append(Deviceid);
     linename.append("下限电压");
     QLocale locale = QLocale::English;//指定英文显示
     int count =0;

     QVector<QDateTime> X;
     QVector<qreal> Y1;
     QVector<qreal> Y2;
     while (m_query->next()) {
         count++;
         QString tmpvoltage = m_query->value(4).toString();//当前电压
         QString lowvoltage = m_query->value(5).toString();//下限电压
         //将英文的日期转换成本地相应的格式
         QDate tmpdate = locale.toDate(m_query->value(9).toString().split("-").at(0),"MMM dd");
         QTime tmptime = QTime::fromString(m_query->value(9).toString().split("-").at(1),"hh:mm");
//          if (tmptime.toString("hh:mm")=="00:00")
//              qDebug() << true;
//          else qDebug() << false;
         QDateTime xValue;
         xValue.setDate(tmpdate);
         xValue.setTime(tmptime);
         tmpvoltage = tmpvoltage.left(tmpvoltage.size()-2);
         lowvoltage = lowvoltage.left(lowvoltage.size()-1);                  
         qreal yValue =  tmpvoltage.toUInt();
         qreal yValue_low = lowvoltage.toUInt()*1000;
         X.append(xValue);
         Y1.append(yValue);
         Y2.append(yValue_low);
     }
    int windowwidth = 0;
    if(Y1.size()>11){
         windowwidth=5;
    }
    else {
       windowwidth=1;
    }
    QVector<qreal> y1value = this->smoothAvgYValue(Y1,windowwidth);
    for (int i =0 ; i <y1value.size();i++) {
        series->append(X.at(i).toMSecsSinceEpoch(),y1value.at(i));
        solidline->append(X.at(i).toMSecsSinceEpoch(),Y2.at(i));
    }

    QList<QLineSeries*> m_series;
    //添加坐标点
    m_series.append(solidline);
    m_series.append(series);
    ui->tableViewhistory->scrollToBottom();
    ui->tableViewhistory->selectRow(count-1);

    m_model->setEditStrategy(QSqlTableModel::OnManualSubmit);

    m_linechart = new LineChart(this);
    m_chartview = new ZoomChartView(m_linechart->getLineChart(m_series,linename),this->ui->graphicsView);

    m_chartview->setMinimumSize(1220,500);
    //抗锯齿
    m_chartview->setRenderHint(QPainter::Antialiasing);
    ui->graphicsView->setMinimumSize(1220,500);
    ui->graphicsView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_chartview->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_chartview->chart()->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_valueLabel = new QLabel(this);
    m_valueLabel->setStyleSheet(QString("QLabel{color:rgb(0,55,200); font-family:\"Microsoft Yahei\"; font-size:12px; font-weight:bold;"
                                        " background-color:rgb(255, 255, 255); border-radius:4px; text-align:center;}"));
    // " background-color:rgba(21, 100, 255, 51); border-radius:4px; text-align:center;}"));
    m_valueLabel->setFixedSize(44, 24);
    m_valueLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
//    m_valueLabel->setStyleSheet("background-color: rgb(255, 255, 255);border-width: 5px");
//    m_valueLabel->setStyleSheet("font-size:20px;color:red;font-family:黑体");
    connect(m_linechart,&LineChart::sigHoveredPoint,this,&HistoryRecord::slotPointHoverd);
}

void HistoryRecord::slotPointHoverd(const QPointF &point, bool state)
{
    if (state) {
        m_valueLabel->setText(QString::asprintf("%1.0f", point.y()));

        QPoint curPos = mapFromGlobal(QCursor::pos());
        m_valueLabel->move(curPos.x() - m_valueLabel->width() / 2, curPos.y() - m_valueLabel->height() * 1.5);//移动数值
        m_valueLabel->show();//显示出来
    }
    else
        m_valueLabel->hide();//进行隐藏
}

void HistoryRecord::resizeEvent(QResizeEvent *event){
    if(event->oldSize()!=QSize(-1, -1))
    {
        ui->tableViewhistory->horizontalHeader()->resizeSections(QHeaderView::Stretch);
        m_chartview->resize(ui->graphicsView->size());
    }
}


QVector<qreal> HistoryRecord::smoothAvgYValue(QVector<qreal> tmpyvalue,int windowWidth) {
    QVector<qreal> returnvector;
    for (int i = 0 ; i<windowWidth; i++) {
        returnvector.append(tmpyvalue[i]);
    }
    for (int i = windowWidth; i < tmpyvalue.size()-windowWidth;i++) {
        int window = windowWidth;
        qreal avgyvalue=0;
        while (window--) {
            avgyvalue+=tmpyvalue.at(i+window);
            avgyvalue+=tmpyvalue.at(i-window);
        }
        avgyvalue+=tmpyvalue.at(windowWidth);
        avgyvalue=avgyvalue/(2*windowWidth+1);
        returnvector.append(avgyvalue);
    }
    for (int i = tmpyvalue.size()-windowWidth ; i < tmpyvalue.size(); i++) {
        returnvector.append(tmpyvalue[i]);
    }
    return returnvector;
}


