#include "MainWindow.h"
#include <QSplineSeries>
#include <QChartView>
#include <QSplitter>
#include <QTimer>
#include <QDebug>

MainWindow::MainWindow(QString fileCur, QWidget *parent)
    : QMainWindow(parent)
{
    m_curMgr.m_sFileCur = fileCur;
    initUI();
//    resize(1000,750);
    showMaximized();
    connect(&m_curMgr, &XCurMgr::signal_DataReady, this, &MainWindow::updateCuveList);
    QTimer::singleShot(0, &m_curMgr, &XCurMgr::openFile);
}

MainWindow::~MainWindow()
{
}

void MainWindow::initUI()
{
    m_chart = new QChart();
    m_chart->legend()->show();
    m_chart->setTitle("曲线图");

//    //添加完addSeries之后才能继续下面的
//    m_chart->createDefaultAxes();
//    m_chart->axisY()->setRange(0, 10);

    QChartView *chartView = new QChartView(m_chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    m_listView_curve = new QListView;
    m_listView_curve->setMinimumWidth(150);
    m_listView_curve->setMaximumWidth(350);
    m_listView_curve->setStyleSheet("QListView {background-color:white;}"
                                   "QListView::item{border:none; height: 35px;}"
                                   "QListView::item:selected{background: rgba(0, 120, 215,200);}"
                                   "QListView::item::hover {background: rgba(0, 120, 215,100);}");
    connect(m_listView_curve, &QListView::clicked, this, &MainWindow::updateCurveChart);

    m_model_curve = new QStringListModel(this);
    m_listView_curve->setModel(m_model_curve);

    QSplitter *splitter = new QSplitter;
    splitter->setOrientation(Qt::Horizontal);
    splitter->addWidget(m_listView_curve);
    splitter->addWidget(chartView);

    setCentralWidget(splitter);

    QPalette pal = window()->palette();
    pal.setColor(QPalette::Window, QRgb(0xf0f0f0));
    pal.setColor(QPalette::WindowText, QRgb(0x404044));
    window()->setPalette(pal);
}

void MainWindow::updateCuveList()
{
    qDebug() << "更新曲线列表，开始...";
    QStringList allTypeName = m_curMgr.m_allCurves.keys();
    m_model_curve->setStringList(allTypeName);
    qDebug() << "更新曲线列表，完成。";
}

void MainWindow::updateCurveChart()
{
    QModelIndex index = m_listView_curve->currentIndex();
    int iRow = index.row();//行号
    if(iRow < 0)
        return;

    QString sCurveName = m_model_curve->stringList().at(iRow);//曲线名称
    XCurve *pCurve = Q_NULLPTR;
    if(m_curMgr.m_allCurves.contains(sCurveName))
        pCurve = m_curMgr.m_allCurves.value(sCurveName);
    if(pCurve)
    {
        m_chart->removeAllSeries();//清空所有曲线
        QSplineSeries *series = new QSplineSeries();
        series->setName(sCurveName);
        QList<QPointF> points = pCurve->m_data;
        if(m_curMgr.m_isBpaOut)//bpa,横轴是周波，要转换成时间，需要除以50
        {
            for(int k = 0; k < points.count(); ++k)
            {
                points[k].setX(points[k].x()/50);//横轴单位从转换成时间
            }
        }
        series->append(points);
        m_chart->addSeries(series);

        //添加完addSeries之后才能继续下面的
        m_chart->createDefaultAxes();

        //计算X,Y轴范围
//        qreal minX = 0,maxX= 0,minY= 0,maxY = 0;
//        if(pCurve->m_data.count() > 0)
//        {
//            minX = maxX = pCurve->m_data.at(0).x();
//            minY = maxY = pCurve->m_data.at(0).y();
//            for(int i = 0; i < pCurve->m_data.count(); ++i)
//            {
//                if(pCurve->m_data.at(0).x() < minX)
//                    minX = pCurve->m_data.at(0).x();
//                else if(pCurve->m_data.at(0).x() > maxX)
//                    maxX = pCurve->m_data.at(0).x();

//                if(pCurve->m_data.at(0).y() < minY)
//                    minY = pCurve->m_data.at(0).y();
//                else if(pCurve->m_data.at(0).y() > maxY)
//                    maxY = pCurve->m_data.at(0).y();
//            }
//        }
//        m_chart->axisX()->setRange(minX, maxX);
//        m_chart->axisY()->setRange(minY, maxY);
    }
}
