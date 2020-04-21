#include "linechart.h"
#include <QDebug>
#include <QDateTimeAxis>
#include <QValueAxis>
#include <QCategoryAxis>
//#include "zoomchart.h"
#include <QDateTime>
#include <QScatterSeries>
#include <QVector>
#include <QSplineSeries>
LineChart::LineChart(QObject *parent) : QObject(parent)
{

}

QChart* LineChart::getLineChart(QList<QLineSeries*> lineseries, QStringList linename){
    //赋值线段列表
    m_series = lineseries;
    QChart *chart = new QChart();
    chart->setTheme(QChart::ChartThemeBlueCerulean);//设置系统主题
    chart->setAnimationOptions(QChart::AllAnimations);//设置启用或禁用动画
    //chart->setBackgroundBrush(QBrush(QColor(170,170,255)));//设置背景色,主题和背景二选一
    //chart->setDropShadowEnabled(true);//是否背景阴影
    chart->setLocalizeNumbers(true);//数字是否本地化
    //设置线条名称
    chart->createDefaultAxes();//创建默认轴,需要在修改自己的坐标轴之前

    //设置X轴为时间轴
    QDateTimeAxis *dateAxisX = new QDateTimeAxis;
    dateAxisX->setFormat("MM-dd hh:mm");
    dateAxisX->setTitleText(QStringLiteral("时间"));

//    QString str = QDate::currentDate().toString("MM-dd");
//    str.append(" 00:00");
//    qDebug() << str;
//    QDateTime todaybegin;
//    todaybegin =  todaybegin.fromString(str,"MM-dd hh:mm");
//    qDebug() << todaybegin.toString();
//    str.remove(5,6);
//    str.append(" 23:59");
//    qDebug() << str;
//    QDateTime todayend;
//    todayend = todayend.fromString(str,"MM-dd hh:mm");
//    dateAxisX->setRange(todaybegin,todayend);
    //下面的这个添加坐标的方法是不推荐的
//      chart->setAxisX(dateAxisX, m_series.at(i));
    //注意，下面两个语句是一起用的
    chart->addAxis(dateAxisX,Qt::AlignBottom);

    QValueAxis *voltageAxisY =new QValueAxis;
    voltageAxisY->setTitleText(QStringLiteral("电压/mv"));
    //设置显示范围
    voltageAxisY->setRange(0,20000);
    voltageAxisY->setTickCount(5);
//        voltageAxisY->setGridLineColor(Qt::red);
    //voltageAxisY->setGridLineVisible(false);
//        voltageAxisY->setLabelFormat("%d");
//        chart->setAxisY(voltageAxisY,m_series.at(i));
//    voltageAxisY->setMinorTickCount(1);
//    voltageAxisY->setMinorGridLineColor(Qt::red);
    chart->addAxis(voltageAxisY,Qt::AlignLeft);

    for(int i = 0;i < m_series.size(); i++){
        m_series.at(i)->setName(linename.at(i));
//        m_series.at(i)->setColor(QColor(255,0,255));
        //设置是否线条可视
        m_series.at(i)->setVisible(true);
        //点标签是否可视
        m_series.at(i)->setPointLabelsVisible(false);
        //点标签颜色
        m_series.at(i)->setPointLabelsColor(QColor(255,255,255));
        //点标签字体
        m_series.at(i)->setPointLabelsFont(QFont("微软雅黑"));
        //设置点标签显示格式
        m_series.at(i)->setPointLabelsFormat("(@xPoint,@yPoint)");
        //是否切割边缘点标签，默认为true
        m_series.at(i)->setPointLabelsClipping(false);
        //设置点是否可视
        m_series.at(i)->setPointsVisible(true);
        //添加很多点到QChart上
        chart->addSeries(m_series.at(i));

        m_series.at(i)->attachAxis(dateAxisX);

        m_series.at(i)->attachAxis(voltageAxisY);

//        QCategoryAxis *axisY3 = new QCategoryAxis;
//        axisY3->append("", 20000);
//        axisY3->setLinePenColor(voltageAxisY->linePenColor());
//        axisY3->setGridLineColor(Qt::red);
//        chart->addAxis(axisY3, Qt::AlignRight);
//        m_series.at(i)->attachAxis(axisY3);
    }

    //为了画10V的线
    m_series.at(0)->setColor(QColor(255,0,0));
    QPen pen = QPen(Qt::SolidLine);
    pen.setWidth(3);
    pen.setColor(Qt::red);
    m_series.at(0)->setPen(pen);

    m_series.at(0)->setPointsVisible(false);
    m_series.at(1)->setColor(Qt::yellow);
//用于鼠标覆盖上去，显示数据的散点
    //空心点
    //外部的点
    QScatterSeries *m_ScatterSeriesOutside = new QScatterSeries;
    m_ScatterSeriesOutside->setMarkerShape(QScatterSeries::MarkerShapeCircle);//圆形的点
    m_ScatterSeriesOutside->setBorderColor(Qt::white);  //边框颜色
    m_ScatterSeriesOutside->setBrush(QBrush(Qt::white));//背景颜色
    m_ScatterSeriesOutside->setMarkerSize(12);                     //点大小
    //内部的点
    QScatterSeries *m_ScatterSeriesInside = new QScatterSeries;
    m_ScatterSeriesInside->setMarkerShape(QScatterSeries::MarkerShapeCircle);//圆形的点
    m_ScatterSeriesInside->setBorderColor(QColor(21, 100, 255));//边框颜色
    m_ScatterSeriesInside->setBrush(QBrush(QColor(21, 100, 255)));//背景颜色
    m_ScatterSeriesInside->setMarkerSize(6);


    QVector<QPointF>::iterator iter;
    QVector<QPointF> pointvector = m_series.at(1)->pointsVector();
    for(iter = pointvector.begin();iter!=pointvector.end();++iter)
    {
        m_ScatterSeriesOutside->append(*iter);
        m_ScatterSeriesInside->append(*iter);
    }
    chart->addSeries(m_ScatterSeriesOutside);
    m_ScatterSeriesOutside->attachAxis(dateAxisX);
    m_ScatterSeriesOutside->attachAxis(voltageAxisY);
    chart->addSeries(m_ScatterSeriesInside);
    m_ScatterSeriesInside->attachAxis(dateAxisX);
    m_ScatterSeriesInside->attachAxis(voltageAxisY);
//将QScatterSeries信号转换成自定义的信号，便于使用
    connect(m_ScatterSeriesInside,&QScatterSeries::hovered,this,&LineChart::slotemitSighoveredPoint);
//    QLineSeries *series1;
//    *series1 = new QLineSeries
//    series1->setBrush(QColor(255,0,255));
//    series1->setPen(QColor(255,0,255));
    chart->setTitle(QString("设备电压历史记录"));//设置标题
    chart->setTitleBrush(QBrush(QColor(255,255,255)));//设置标题的刷子和颜色
    chart->setTitleFont(QFont("微软雅黑"));//设置标题字体

    //下面的都是图示，底部对齐
    chart->legend()->setAlignment(Qt::AlignBottom);
    //chart->legend()->detachFromChart();
    chart->legend()->setBackgroundVisible(true);//设置背景是否可视
    //chart->legend()->setAutoFillBackground(true);//设置背景自动填充
    //chart->legend()->setColor(QColor(255,128,128,128));//设置颜色
    //chart->legend()->setContentsMargins(10,10,10,10);//设置边距left,top,right,bottom
    chart->legend()->setLabelColor(QColor(255,128,255));//设置标签颜色
    chart->legend()->setVisible(true);//设置是否可视
    //chart->legend()->setMaximumHeight(50);
    //chart->legend()->setMaximumWidth(120);
    //chart->legend()->setMaximumSize(10000);
    //chart->legend()->setGeometry(50,50,120,50);//设置几何位置x,y,w,h
    //chart->legend()->setBrush(QBrush(QColor(128,128,128,128)));
    //chart->legend()->setPen(QPen(QColor(192,192, 192,192)));
    chart->legend()->setBorderColor(QColor(255,255,170,185));//设置边框颜色
    QFont font = chart->legend()->font();
    font.setItalic(!font.italic());
    chart->legend()->setFont(font);//设置字体为斜体
    font.setPointSizeF(12);
    chart->legend()->setFont(font);//设置字体大小
    chart->legend()->setFont(QFont("微软雅黑"));//设置字体类型
    //图示不显示
    chart->legend()->hide();//legend是否显示，show和hide
    return chart;
//    QChartView *chartView = new QChartView(chart,this->ui->widget);
//    chartView->resize(1000,500);
}


void LineChart::slotemitSighoveredPoint(const QPointF &point, bool state)
{
   emit this->sigHoveredPoint(point,state);
}
