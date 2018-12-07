#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "XCurMgr.h"
#include <QMainWindow>

#include <QChart>
#include <QListView>
#include <QStringListModel>
#include <QMovie>
#include <QLabel>

QT_CHARTS_USE_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QString fileCur, QWidget *parent = 0);
    ~MainWindow();

    void initUI();

public Q_SLOTS:
    void showWaitingDlg();
    void updateWaitingDlg();
    void hideWaitingDlg();
    void updateCuveList();
    void updateCurveChart();

protected:
    virtual void resizeEvent(QResizeEvent *event);

private:
    XCurMgr m_curMgr;//曲线管理器

    QChart *m_chart;
    QListView *m_listView_curve;
    QStringListModel *m_model_curve;
    QMovie *m_pMovieWaiting;
    QLabel *m_pLabelWaiting;
};

#endif // MAINWINDOW_H
