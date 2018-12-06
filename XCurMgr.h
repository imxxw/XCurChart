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
    XCurveMap m_allCurves;//所有的曲线
    QThread m_thread;//后台处理的线程

    bool m_isBpaOut;//是否是BPA输出的曲线文件（BPA输出的曲线，时间保存的是周波，而全过程输出的曲线，时间保存的是真实时间）
};

#endif // XCURMGR_H
