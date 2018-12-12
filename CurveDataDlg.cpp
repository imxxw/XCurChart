#include "CurveDataDlg.h"
#include <QVBoxLayout>
#include <QTimer>

CurveDataDlg::CurveDataDlg(QCustomPlot *plot, QWidget *parent)
    : QDialog(parent),m_plot(plot)
{
    setWindowFlags(windowFlags()|Qt::WindowMaximizeButtonHint);//可改变大小
    setWindowTitle("曲线数据浏览器");
    initUI();
    resize(700,600);
    QTimer::singleShot(10, this, &CurveDataDlg::updateCurveList);
}


void CurveDataDlg::initUI()
{
    m_model_curve = new QStringListModel(this);
    m_listView_curve = new QListView;
    m_listView_curve->setModel(m_model_curve);
    //    m_listView_curve->setSelectionMode(QAbstractItemView::MultiSelection);
    m_listView_curve->setEditTriggers(QAbstractItemView::NoEditTriggers);//不可编辑
    m_listView_curve->setSpacing(0);
    m_listView_curve->setMinimumWidth(300);
    m_listView_curve->setMaximumWidth(500);
    m_listView_curve->setStyleSheet("QListView {background-color:white;}"
                                    "QListView::item{border:none; height: 22px;}"
                                    "QListView::item:selected{background: rgba(0, 120, 215,200);}"
                                    "QListView::item::hover {background: rgba(0, 120, 215,100);}");
    connect(m_listView_curve, &QListView::clicked, this, &CurveDataDlg::updateTable);

    m_tableModel_curveData = new CurveDataTableModel(this);
    m_tableView_data = new QTableView;
    m_tableView_data->setModel(m_tableModel_curveData);
    m_tableView_data->setMinimumWidth(200);

    QSplitter *splitter = new QSplitter;
    splitter->setOrientation(Qt::Horizontal);
    splitter->addWidget(m_listView_curve);
    splitter->addWidget(m_tableView_data);
    splitter->setStyleSheet("QSplitter:handle{background-color:lightgray;}");

    QVBoxLayout *vLayoutMain = new QVBoxLayout;
    vLayoutMain->addWidget(splitter);
    setLayout(vLayoutMain);
}

QStringList CurveDataDlg::allCurveNames()
{
    QStringList allNames;
    if(m_plot && m_plot->graphCount())
    {
        for(int i = 0; i < m_plot->graphCount(); ++i)
        {
            allNames.append(m_plot->graph(i)->name());
        }
    }
    return  allNames;
}

void CurveDataDlg::updateCurveList()
{
    m_model_curve->setStringList(allCurveNames());
    if(m_model_curve->stringList().count() > 0)
    {
        QModelIndex index = m_model_curve->index(0);
        m_listView_curve->setCurrentIndex(index);//默认选中第一行
        updateTable();
    }
}

void CurveDataDlg::updateTable()
{
    if(!m_plot)
        return;

    QModelIndex index = m_listView_curve->currentIndex();
    int iRow = index.row();//行号
    if(iRow < 0)
        return;
    if(iRow >= m_plot->graphCount())
        return;

    QCPGraph *graph = Q_NULLPTR;
    graph = m_plot->graph(iRow);
    m_tableModel_curveData->setModelData(graph);
}

void CurveDataDlg::updateUI()
{
    updateCurveList();
    updateTable();
}
