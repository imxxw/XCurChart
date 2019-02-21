#ifndef MAINWINDOW_201812112212_H
#define MAINWINDOW_201812112212_H

#include "XCurMgr.h"
#include "XxwCustomPlot.h"
#include <QMainWindow>
#include <QListView>
#include <QStringListModel>
#include <QMovie>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QMap>

class MainWnd : public QMainWindow
{
    Q_OBJECT

public:
    MainWnd(QString fileCur, QWidget *parent = 0);
    ~MainWnd();

    void initUI();
    void initPlot();

    void addCurveToPlot(const QString &sTypeName);

    // 生成一个随机颜色，为了在白色背景上显示，尽量生成深色
    QColor getRandomColor();

    ///
    /// \brief filterCurveByKey:通过关键字来筛选曲线名称，
    /// 即查找曲线名称中是否含有关键字，可能有多个关键字，采用逗号分割，只要包含其中一个关键字，就表示符合要求
    /// \param curveName：曲线名称
    /// \param key：关键字
    /// \return bool:true表示含有该关键字，反之就不含该关键字
    ///
    bool filterCurveByKey(const QString &curveName, const QString &key);

public Q_SLOTS:
    void selectFileCur();
    void openFileCur();
    void showWaitingDlg();
    void updateWaitingDlg();
    void hideWaitingDlg();
    void updateCurveList();
    void changeFilterKey(const QString &);
    void filterCurve();
    void resotreCurve();
    void addCurve();
    void removeCurve(QString curveName);
    void removeAllCurves();
    void changeFileCur(const QString &);
    void graphClicked(QCPAbstractPlottable *plottable, int dataIndex);
    void selectionChanged();

protected:
    virtual void resizeEvent(QResizeEvent *event);

private:
    QString m_sFileCur;//曲线文件
    XCurMgr m_curMgr;//曲线管理器

    QComboBox *m_comboBox_fileCurve;//曲线名称下拉框
    QPushButton *m_pushButton_selectFile;//选择曲线按钮
    QComboBox *m_comboBoxFilterKey;
    QPushButton *m_buttonFilter;
    QPushButton *m_buttonRestore;

//    QPushButton *m_buttonRemoveSelectedCurve;
//    QPushButton *m_buttonRemoveAllCurves;
//    QPushButton *m_buttonViewCurveData;
//    QPushButton *m_buttonOutputCurveData;
//    QPushButton *m_buttonOutputPicture;

    QListView *m_listView_curve;//可选的曲线列表
    QStringList m_allTypeName;//所有的曲线类型和名称列表
    QStringList m_filteredTypeName;//筛选后的曲线类型和名称列表
    QStringList m_selectedTypeName;//选择的曲线类型和名称列表
    QStringListModel *m_model_curve;//可选的曲线列表模型
    XxwCustomPlot *m_plot;//图表

    QMovie *m_pMovieWaiting;
    QLabel *m_pLabelWaiting;
};

#endif // MAINWINDOW_201812112212_H
