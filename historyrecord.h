#ifndef HISTORYRECORD_H
#define HISTORYRECORD_H

#include <QWidget>
#include "tabmodelalicenter.h"
#include <QSqlQuery>
#include "linechart.h"
#include <QChartView>//显示图表
#include <QLineSeries>//线系列
#include "zoomchartview.h"
QT_CHARTS_USE_NAMESPACE
namespace Ui {
class HistoryRecord;
}

class HistoryRecord : public QWidget
{
    Q_OBJECT

public:
    explicit HistoryRecord(QWidget *parent = nullptr);
    ~HistoryRecord();

    void showHistoryRecord(QString Deviceid, QSqlDatabase *db);
    void resizeEvent(QResizeEvent *event);
    QVector<qreal> smoothAvgYValue(QVector<qreal> tmplineseries,int windowWidth);
private slots:
    void slotPointHoverd(const QPointF &point, bool state);


private:
    Ui::HistoryRecord *ui;

    TabModelAliCenter *m_model;
    QSqlQuery *m_query;
    QSqlDatabase *m_db;
    LineChart *m_linechart;

    ZoomChartView *m_chartview;

    QLabel *m_valueLabel;

};

#endif // HISTORYRECORD_H
