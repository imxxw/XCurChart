#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "XCurMgr.h"
#include <QMainWindow>

#include <QChart>
#include <QListView>
#include <QStringListModel>
#include <QMovie>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>

QT_CHARTS_USE_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QString fileCur, QWidget *parent = 0);
    ~MainWindow();

    void initUI();

public Q_SLOTS:
    void selectFileCur();
    void openFileCur();
    void showWaitingDlg();
    void updateWaitingDlg();
    void hideWaitingDlg();
    void updateCuveList();
    void selectCurve();
    void removeCurve();
    void clearCurve();
    void fileCurChanged(const QString &);

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
    QChart *m_chart;//图表

    QMovie *m_pMovieWaiting;
    QLabel *m_pLabelWaiting;
};

#endif // MAINWINDOW_H
