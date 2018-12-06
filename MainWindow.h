#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "XCurMgr.h"
#include <QMainWindow>

#include <QChart>
#include <QListView>
#include <QStringListModel>

QT_CHARTS_USE_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QString fileCur, QWidget *parent = 0);
    ~MainWindow();

    void initUI();

public Q_SLOTS:
    void updateCuveList();
    void updateCurveChart();

private:
    XCurMgr m_curMgr;//曲线管理器

    QChart *m_chart;
    QListView *m_listView_curve;
    QStringListModel *m_model_curve;
};

#endif // MAINWINDOW_H
