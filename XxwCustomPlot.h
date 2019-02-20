#ifndef XCUSTOMPLOT_H
#define XCUSTOMPLOT_H

#include "XxwTracer.h"
#include "qcustomplot.h"
#include <QObject>
#include <QList>

class XxwCustomPlot:public QCustomPlot
{
    Q_OBJECT

public:
    XxwCustomPlot(QWidget *parent = 0);

    void initPlot();
protected:
    virtual void mouseMoveEvent(QMouseEvent *event);

public:
    ///
    /// \brief 设置是否显示鼠标追踪器
    /// \param show:是否显示
    ///
    void showTracer(bool show)
    {
        m_isShowTracer = show;
        //不显示x,y轴上的追踪器
//        if(m_xTracer)
//            m_xTracer->setVisible(false);
//        if(m_yTracer)
//            m_yTracer->setVisible(false);
        foreach (XxwTracer *tracer, m_dataTracers)
        {
            if(tracer)
                tracer->setVisible(m_isShowTracer);
        }
        if(m_lineTracer)
            m_lineTracer->setVisible(m_isShowTracer);
    }

    ///
    /// \brief 是否显示鼠标追踪器
    /// \return
    ///
    bool isShowTracer(){return m_isShowTracer;};    

    ///
    /// \brief 设置是否突出显示曲线上的点（圆圈显示）
    /// \param show：是否显示
    ///
    void showScatter(bool show);

    ///
    /// \brief 是否突出显示曲线上的点（圆圈显示）
    /// \return
    ///
    bool isShowScatter(){return m_isShowScatter;}

    void saveTxt(const QString &fileName);

    void saveCsv(const QString &fileName);

public slots:
    void selectionChanged();
    void mousePress();
    void mouseWheel();
    void titleDoubleClick(QMouseEvent *event);

    void axisLabelDoubleClick(QCPAxis* axis, QCPAxis::SelectablePart part);
    void legendDoubleClick(QCPLegend* legend, QCPAbstractLegendItem* item);

//    void addRandomGraph();
    void removeSelectedGraph();
    void removeAllGraphs();
    void contextMenuRequest(QPoint pos);
    void moveLegend();

    //自动调整坐标轴，以显示所有图形
    void autoScaleAxises();

    //查看图形数据
    void viewGrpahData();
    
    //导出图形数据
    void outputGraphData();

    //复制（复制图标到剪切板）
    void copyToClipboard();

    //导出图形
    void outputPlot();

    //跟随鼠标显示曲线上的值
    void showValueAtMousePoint();

    //突出显示曲线上的点（圆圈显示）
    void showScatterOfCurves();

signals:
    void signal_removeSelectedGraph(QString);
    void signal_removeAllGraphs();

private:
    bool m_isShowTracer;//是否显示追踪器（鼠标在图中移动，显示对应的值）

    bool m_isShowScatter;//显示曲线上的点
//    XxwTracer *m_xTracer;//x轴
//    XxwTracer *m_yTracer;//y轴
    QList<XxwTracer *> m_dataTracers;//
    XxwTraceLine  *m_lineTracer;//直线
};

#endif // XCUSTOMPLOT_H
