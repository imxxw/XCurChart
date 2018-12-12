#ifndef CURVEDATADLG_H
#define CURVEDATADLG_H

#include "qcustomplot.h"
#include "CurveDataTableModel.h"
#include <QDialog>
#include <QMainWindow>
#include <QListView>
#include <QStringListModel>
#include <QTableView>
#include <QPushButton>

///
/// \brief The CurveDataDlg class:曲线数据浏览类
///
class CurveDataDlg : public QDialog
{
    Q_OBJECT
public:
    explicit CurveDataDlg(QCustomPlot *plot,QWidget *parent = nullptr);
    void initUI();

    QStringList allCurveNames();

signals:

public slots:
    void updateUI();
    void updateCurveList();
    void updateTable();

private:
    QCustomPlot *m_plot;
    QStringListModel *m_model_curve;//可选的曲线列表模型
    QListView *m_listView_curve;//可选的曲线列表
    CurveDataTableModel *m_tableModel_curveData;
    QTableView *m_tableView_data;
};

#endif // CURVEDATADLG_H
