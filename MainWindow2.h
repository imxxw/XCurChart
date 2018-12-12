#ifndef MAINWINDOW_201812112212_H
#define MAINWINDOW_201812112212_H

#include "XCurMgr.h"
#include "XxwCustomPlot.h"
#include <QMainWindow>
#include <QChart>
#include <QChartView>
#include <QListView>
#include <QStringListModel>
#include <QMovie>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QMap>
#include <QLineSeries>

QT_CHARTS_USE_NAMESPACE

class MainWindow2 : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow2(QString fileCur, QWidget *parent = 0);
    ~MainWindow2();

    void initUI();
    void initPlot();

    void addCurveToChart(const QString &sTypeName);

    // 生成一个随机颜色，为了在白色背景上显示，尽量生成深色
    QColor getRandomColor();

public Q_SLOTS:
    void selectFileCur();
    void openFileCur();
    void showWaitingDlg();
    void updateWaitingDlg();
    void hideWaitingDlg();
    void updateCuveList();
    void addCurve();
    void removeCurve();
    void clearCurve();
    void fileCurChanged(const QString &);

    void graphClicked(QCPAbstractPlottable *plottable, int dataIndex);

protected:
    virtual void resizeEvent(QResizeEvent *event);

private:
    QString m_sFileCur;//曲线文件
    XCurMgr m_curMgr;//曲线管理器

    QComboBox *m_comboBox_fileCurve;//曲线名称下拉框
    QPushButton *m_pushButton_selectFile;//选择曲线按钮

    QListView *m_listView_curve;//可选的曲线列表
    QStringListModel *m_model_curve;//可选的曲线列表模型
    QListView *m_listView_curve_selected;//已选的曲线列表
    QStringListModel *m_model_curve_selected;//已选的曲线列表模型
    QPushButton *m_pushButton_ClearCurve;
    XxwCustomPlot *m_plot;//图表

    QMovie *m_pMovieWaiting;
    QLabel *m_pLabelWaiting;
};

#endif // MAINWINDOW_201812112212_H
