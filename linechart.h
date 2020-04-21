#ifndef LINECHART_H
#define LINECHART_H

#include <QObject>
#include <QChartView>//显示图表
#include <QLineSeries>//线系列
#include <QLabel>
QT_CHARTS_USE_NAMESPACE
class LineChart : public QObject
{
    Q_OBJECT
public:
    explicit LineChart(QObject *parent = nullptr);

    QChart* getLineChart(QList<QLineSeries*> lineseries, QStringList linename);

signals:
    void sigHoveredPoint(const QPointF &point, bool state);

public slots:
    void slotemitSighoveredPoint(const QPointF &point, bool state);

private:
    QList<QLineSeries*> m_series;


};

#endif // LINECHART_H
