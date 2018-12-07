#ifndef XCURMGR_H
#define XCURMGR_H

#include <QObject>
#include <QString>
#include <QList>
#include <QPointF>
#include <QMap>
#include <QThread>

///
/// \brief 曲线类
///
class XCurve
{
public:
    QString m_sName;//名称
    QString m_sType;//类型
    QList<QPointF> m_data;//曲线上的点
};
typedef QList<XCurve *> XCurvePtrList;//曲线指针列表
typedef QMap<QString, XCurve *> XCurveMap;//曲线名到曲线指针映射

///
/// \brief 采用qt类的曲线文件管理器
///
class XCurMgr:public QObject
{
    Q_OBJECT
public:
    XCurMgr();
    ~XCurMgr()
    {
        releaseAll();
        m_thread.quit();
        m_thread.wait();
    }

    void releaseAll();

public Q_SLOTS:
    void openFile();

Q_SIGNALS:
    void signal_DataReady();//数据准备好了

public:
    QString m_sFileCur;//曲线文件
    int m_nTag;//tag（cur文件的标志，常量：960829，如果不是960829,就表示不是正确的cur文件）
    int m_nSampleCount;//采样点的总数，同一个曲线文件中所有的曲线的采样点数都一样
    QList<float> m_Times;//时间信息（实际上，采样间隔前后不一定相同）
    QStringList m_Comments;//描述信息
    QList<XCurve *> m_curvesList;//曲线列表
    XCurveMap m_curvesMap;//曲线映射（key:type+name, value:曲线指针）
    QThread m_thread;//后台处理的线程

    bool m_isBpaOut;//是否是BPA输出的曲线文件（BPA输出的曲线，时间保存的是周波，而全过程输出的曲线，时间保存的是真实时间）
};

#endif // XCURMGR_H
