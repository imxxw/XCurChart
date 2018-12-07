#include "MainWindow.h"
#include <QSplineSeries>
#include <QChartView>
#include <QSplitter>
#include <QTimer>
#include <QDebug>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QFileDialog>


MainWindow::MainWindow(QString fileCur, QWidget *parent)
    : QMainWindow(parent)
{
    initUI();
//    resize(1000,750);
    showMaximized();

    connect(&m_curMgr, &XCurMgr::signal_DataReady, this, &MainWindow::updateCuveList);

    QString sTitle = "PSD-BPA稳定计算结果文件(*.cur)绘图工具";
    m_sFileCur = "";
    fileCur = fileCur.trimmed();
    if(!fileCur.isEmpty() && QFile::exists(fileCur))
    {
        m_sFileCur = fileCur;
        sTitle += "-";
        sTitle += m_sFileCur;
        openFileCur();//打开cur文件
    }
    setWindowTitle(sTitle);
}

MainWindow::~MainWindow()
{
}

void MainWindow::initUI()
{
    QWidget *widgetLeft = new QWidget;
    widgetLeft->setMinimumWidth(200);
    widgetLeft->setMaximumWidth(500);
    QVBoxLayout *vBoxLayoutLeft = new QVBoxLayout(widgetLeft);
//    widgetLeft->setLayout(vBoxLayoutLeft);
    vBoxLayoutLeft->setMargin(2);
    vBoxLayoutLeft->setSpacing(5);

    QLabel *labelFileCurve = new QLabel("曲线文件:");
    labelFileCurve->setMinimumWidth(60);
    labelFileCurve->setMaximumWidth(60);
    m_comboBox_fileCurve = new QComboBox;//曲线名称下拉框
    if(QFile::exists(m_sFileCur))
        m_comboBox_fileCurve->addItem(m_sFileCur);
    connect(m_comboBox_fileCurve, SIGNAL(currentIndexChanged(const QString &)),
            this, SLOT(fileCurChanged(const QString &)));
    m_pushButton_selectFile = new QPushButton("选择");//选择曲线按钮
    m_pushButton_selectFile->setMinimumWidth(50);
    m_pushButton_selectFile->setMaximumWidth(50);
    connect(m_pushButton_selectFile, &QPushButton::clicked, this, &MainWindow::selectFileCur);
    QHBoxLayout *layoutFileCurve = new QHBoxLayout();
    layoutFileCurve->addWidget(labelFileCurve);
    layoutFileCurve->addWidget(m_comboBox_fileCurve);
    layoutFileCurve->addWidget(m_pushButton_selectFile);
    layoutFileCurve->setMargin(1);
    layoutFileCurve->setSpacing(1);
    vBoxLayoutLeft->addLayout(layoutFileCurve);

    QVBoxLayout *vBoxLayoutCurve = new QVBoxLayout();
    vBoxLayoutCurve->setMargin(1);
    vBoxLayoutCurve->setSpacing(0);
    QLabel *labelCurveList = new QLabel("可选曲线(双击选择曲线，并添加右侧图中，最多可选6条曲线)");
    vBoxLayoutCurve->addWidget(labelCurveList);
    m_listView_curve = new QListView;
//    m_listView_curve->setSelectionMode(QAbstractItemView::MultiSelection);
    m_listView_curve->setEditTriggers(QAbstractItemView::NoEditTriggers);//不可编辑
    m_listView_curve->setSpacing(0);
    m_listView_curve->setMinimumWidth(150);
//    m_listView_curve->setMaximumWidth(450);
    m_listView_curve->setStyleSheet("QListView {background-color:white;}"
                                   "QListView::item{border:none; height: 22px;}"
                                   "QListView::item:selected{background: rgba(0, 120, 215,200);}"
                                   "QListView::item::hover {background: rgba(0, 120, 215,100);}");
    connect(m_listView_curve, &QListView::doubleClicked, this, &MainWindow::selectCurve);
    m_model_curve = new QStringListModel(this);
    m_listView_curve->setModel(m_model_curve);
    vBoxLayoutCurve->addWidget(m_listView_curve);
    vBoxLayoutLeft->addLayout(vBoxLayoutCurve);

    QVBoxLayout *vBoxLayoutCurveSelected = new QVBoxLayout();
    vBoxLayoutCurveSelected->setMargin(1);
    vBoxLayoutCurveSelected->setSpacing(0);
    QLabel *labelCurveSelected = new QLabel("已选曲线(双击删除曲线，并从右侧图中删除对应曲线)");
    m_pushButton_ClearCurve = new QPushButton("清空曲线");
    m_pushButton_ClearCurve->setMinimumWidth(80);
    m_pushButton_ClearCurve->setMaximumWidth(80);
    m_pushButton_ClearCurve->setEnabled(false);//初始化不可用
    connect(m_pushButton_ClearCurve, &QPushButton::clicked, this, &MainWindow::clearCurve);
    QHBoxLayout *hBoxLayout1 = new QHBoxLayout();
    hBoxLayout1->setMargin(0);
    hBoxLayout1->setSpacing(1);
    hBoxLayout1->addWidget(labelCurveSelected);
    hBoxLayout1->addWidget(m_pushButton_ClearCurve);
    vBoxLayoutCurveSelected->addLayout(hBoxLayout1);
    m_listView_curve_selected = new QListView;
//    m_listView_curve_selected->setSelectionMode(QAbstractItemView::MultiSelection);
    m_listView_curve_selected->setEditTriggers(QAbstractItemView::NoEditTriggers);//不可编辑
    m_listView_curve_selected->setSpacing(0);
//    m_listView_curve_selected->setMaximumHeight(100);
//    m_listView_curve_selected->setMaximumHeight(200);
    m_listView_curve_selected->setStyleSheet("QListView {background-color:white;}"
                                   "QListView::item{border:none; height: 22px;}"
                                   "QListView::item:selected{background: rgba(0, 120, 215,200);}"
                                   "QListView::item::hover {background: rgba(0, 120, 215,100);}");
    connect(m_listView_curve_selected, &QListView::doubleClicked, this, &MainWindow::removeCurve);
    m_model_curve_selected = new QStringListModel(this);
    m_listView_curve_selected->setModel(m_model_curve_selected);
    vBoxLayoutCurveSelected->addWidget(m_listView_curve_selected,1);
    vBoxLayoutLeft->addLayout(vBoxLayoutCurveSelected);

    m_chart = new QChart();
    m_chart->legend()->show();
    m_chart->setTitle("曲线图");
//    //添加完addSeries之后才能继续下面的
//    m_chart->createDefaultAxes();
//    m_chart->axisX()->setTitleVisible(true);
//    m_chart->axisX()->setTitleText("时间(s)");
//    m_chart->axisY()->setRange(0, 10);
    QChartView *chartView = new QChartView(m_chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    vBoxLayoutLeft->setStretch(1,4);
    vBoxLayoutLeft->setStretch(2,1);

    QSplitter *splitter = new QSplitter;
    splitter->setOrientation(Qt::Horizontal);
    splitter->addWidget(widgetLeft);
    splitter->addWidget(chartView);
    splitter->setStyleSheet("QSplitter:handle{background-color:white;}");

    setCentralWidget(splitter);

    QPalette pal = window()->palette();
    pal.setColor(QPalette::Window, QRgb(0xf0f0f0));
    pal.setColor(QPalette::WindowText, QRgb(0x404044));
    window()->setPalette(pal);

    m_pMovieWaiting = new QMovie(":/Res/loading.gif");
    m_pLabelWaiting = new QLabel(this);
    m_pLabelWaiting->setWindowFlag(Qt::FramelessWindowHint);
    int wid = 100, hei = 100;
    m_pLabelWaiting->setFixedSize(wid, hei);
    m_pLabelWaiting->setScaledContents(true);
    m_pLabelWaiting->setMovie(m_pMovieWaiting);
    m_pMovieWaiting->start();
    m_pLabelWaiting->hide();
}


void MainWindow::selectFileCur()
{
    QFileInfo fi(m_sFileCur);
    QString sFileNew = QFileDialog::getOpenFileName(this
                                                    ,"选择曲线文件"
                                                    ,fi.absolutePath()
                                                    ,"曲线文件 (*.cur)");
    if(!sFileNew.isEmpty() && QFile::exists(sFileNew) && sFileNew != m_sFileCur)
    {
        m_sFileCur = sFileNew;
        if(m_comboBox_fileCurve->findText(m_sFileCur) < 0)
            m_comboBox_fileCurve->addItem(sFileNew);//新的曲线文件
        m_comboBox_fileCurve->setCurrentText(sFileNew);
        openFileCur();
    }
}

void MainWindow::openFileCur()
{
    clearCurve();//删除所有的曲线
    if(QFile::exists(m_sFileCur))
    {
        m_curMgr.m_sFileCur = m_sFileCur;
        QTimer::singleShot(0, &m_curMgr, &XCurMgr::openFile);//在子线程中读取曲线文件
        showWaitingDlg();//显示等待窗口
    }
}

void MainWindow::showWaitingDlg()
{
    if(m_pLabelWaiting)
    {
        m_pLabelWaiting->show();
        updateWaitingDlg();
    }
}

void MainWindow::updateWaitingDlg()
{
    if(m_pLabelWaiting)
    {
        m_pLabelWaiting->move((this->width()-m_pLabelWaiting->width())/2, (this->height()-m_pLabelWaiting->height())/2);
    }
}

void MainWindow::hideWaitingDlg()
{
    if(m_pLabelWaiting)
    {
        m_pLabelWaiting->hide();
    }
}

void MainWindow::updateCuveList()
{
    hideWaitingDlg();

    qDebug() << "更新曲线列表，开始...";
    QStringList allTypeName = m_curMgr.m_curvesMap.keys();
    m_model_curve->setStringList(allTypeName);
    qDebug() << "更新曲线列表，完成。";
}

void MainWindow::selectCurve()
{
    if(m_model_curve_selected->stringList().count() >= 5)
    {
        QString sMsg = "已经选择了5条曲线，最多选择5条曲线。";
        QMessageBox::information(this, "提示", sMsg);
        return;
    }

    QModelIndex index = m_listView_curve->currentIndex();
    int iRow = index.row();//行号
    if(iRow < 0)
        return;
    QString sTypeName = m_model_curve->stringList().at(iRow);//曲线名称
    if(sTypeName.isEmpty())
        return;

    XCurve *pCurve = Q_NULLPTR;
    if(m_curMgr.m_curvesMap.contains(sTypeName))
        pCurve = m_curMgr.m_curvesMap.value(sTypeName);
    if(pCurve)
    {
        //更新可选曲线列表
        QStringList curves = m_model_curve->stringList();
        curves.removeAt(iRow);
        m_model_curve->setStringList(curves);

        //更新已选曲线列表
        QStringList curvesSelected = m_model_curve_selected->stringList();
        curvesSelected.append(sTypeName);
        m_model_curve_selected->setStringList(curvesSelected);

        //添加到图中
        QSplineSeries *series = new QSplineSeries();
        series->setName(sTypeName);
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
        m_chart->createDefaultAxes();//创建默认坐标轴
    }

    m_pushButton_ClearCurve->setEnabled(m_model_curve_selected->stringList().count() > 0);
}


void MainWindow::removeCurve()
{
    QModelIndex index = m_listView_curve_selected->currentIndex();
    int iRow = index.row();//行号
    if(iRow < 0)
        return;
    QString sTypeName = m_model_curve_selected->stringList().at(iRow);//曲线名称
    if(sTypeName.isEmpty())
        return;

    //更新可选曲线列表
    QStringList curves = m_model_curve->stringList();
    curves.append(sTypeName);
    m_model_curve->setStringList(curves);

    //更新已选曲线列表
    QStringList curvesSelected = m_model_curve_selected->stringList();
    curvesSelected.removeAt(iRow);
    m_model_curve_selected->setStringList(curvesSelected);

    //找到对应的曲线并删除
    int nSeries = m_chart->series().count();
    for(int i = 0; i < nSeries; ++i)
    {
        if(m_chart->series().at(i)->name() == sTypeName)
        {
            m_chart->removeSeries(m_chart->series().at(i));
            break;
        }
    }
    m_chart->createDefaultAxes();//创建默认坐标轴
    m_pushButton_ClearCurve->setEnabled(m_model_curve_selected->stringList().count() > 0);
}


void MainWindow::clearCurve()
{
    QStringList curvesToClear = m_model_curve_selected->stringList();

    //更新可选曲线列表
    QStringList curves = m_model_curve->stringList();
    foreach(QString sTypeName, curvesToClear)
        curves.append(sTypeName);
    m_model_curve->setStringList(curves);

    //更新已选曲线列表
    m_model_curve_selected->setStringList(QStringList());

    //找到对应的曲线并删除
    m_chart->removeAllSeries();//删除所有的曲线
    m_chart->createDefaultAxes();//创建默认坐标轴
    m_pushButton_ClearCurve->setEnabled(m_model_curve_selected->stringList().count() > 0);
}

void MainWindow::fileCurChanged(const QString &file)
{
    if(m_sFileCur != file)
    {
        m_sFileCur = file;
        openFileCur();
    }
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    updateWaitingDlg();
}
