#include "MainWnd.h"
#include <QSplitter>
#include <QTimer>
#include <QDebug>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QFileDialog>

const int MAX_CURVE_COUNT = 6;//显示曲线条数的最大值

MainWnd::MainWnd(QString fileCur, QWidget *parent)
    : QMainWindow(parent)
{
    initUI();
    //    resize(1000,750);
    showMaximized();

    connect(&m_curMgr, &XCurMgr::signal_DataReady, this, &MainWnd::updateCurveList);

    QString sTitle = "PSD-BPA稳定计算结果文件(*.cur)绘图工具";
    m_sFileCur = "";
    fileCur = fileCur.trimmed();
    if(!fileCur.isEmpty() && QFile::exists(fileCur))
    {
        m_sFileCur = fileCur;
        sTitle += "-";
        sTitle += m_sFileCur;
        if(m_comboBox_fileCurve->findText(m_sFileCur) < 0)
            m_comboBox_fileCurve->addItem(m_sFileCur);//新的曲线文件
        m_comboBox_fileCurve->setCurrentText(m_sFileCur);
        openFileCur();//打开cur文件
    }
    setWindowTitle(sTitle);
}

MainWnd::~MainWnd()
{
}

void MainWnd::initUI()
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
            this, SLOT(changeFileCur(const QString &)));
    m_pushButton_selectFile = new QPushButton("选择");//选择曲线按钮
    m_pushButton_selectFile->setMinimumWidth(50);
    m_pushButton_selectFile->setMaximumWidth(50);
    connect(m_pushButton_selectFile, &QPushButton::clicked, this, &MainWnd::selectFileCur);
    QHBoxLayout *hLayoutFileCurve = new QHBoxLayout();
    hLayoutFileCurve->setMargin(1);
    hLayoutFileCurve->setSpacing(1);
    hLayoutFileCurve->addWidget(labelFileCurve);
    hLayoutFileCurve->addWidget(m_comboBox_fileCurve);
    hLayoutFileCurve->addWidget(m_pushButton_selectFile);
    vBoxLayoutLeft->addLayout(hLayoutFileCurve);

    QVBoxLayout *vBoxLayoutCurve = new QVBoxLayout();
    vBoxLayoutCurve->setMargin(1);
    vBoxLayoutCurve->setSpacing(0);
    QLabel *labelCurveList = new QLabel("可选曲线(双击选择曲线，最多可选5条曲线)");
    vBoxLayoutCurve->addWidget(labelCurveList);

    QLabel *labelFilterKey = new QLabel("关键字:");
    labelFilterKey->setMaximumWidth(45);
    m_comboBoxFilterKey = new QComboBox;
    m_comboBoxFilterKey->setEditable(true);
    m_comboBoxFilterKey->setToolTip("可输入多个关键字，中间用英文逗号\",\"分隔，表示\"或\"的关系");
//    connect(m_comboBoxFilterKey, &QComboBox::editTextChanged, this, &MainWindow2::changeFilterKey);
    connect(m_comboBoxFilterKey, &QComboBox::currentTextChanged, this, &MainWnd::changeFilterKey);
    m_buttonFilter = new QPushButton("筛选");
    m_buttonFilter->setMinimumWidth(50);
    m_buttonFilter->setMaximumWidth(50);
    m_buttonFilter->setEnabled(false);
    connect(m_buttonFilter, &QPushButton::clicked, this, &MainWnd::filterCurve);
    m_buttonRestore = new QPushButton("还原");
    m_buttonRestore->setMinimumWidth(50);
    m_buttonRestore->setMaximumWidth(50);
    m_buttonRestore->setEnabled(false);
    connect(m_buttonRestore, &QPushButton::clicked, this, &MainWnd::resotreCurve);
    QHBoxLayout *hLayoutFilter = new QHBoxLayout();
    hLayoutFilter->setMargin(1);
    hLayoutFilter->setSpacing(1);
    hLayoutFilter->addWidget(labelFilterKey);
    hLayoutFilter->addWidget(m_comboBoxFilterKey);
    hLayoutFilter->addWidget(m_buttonFilter);
    hLayoutFilter->addWidget(m_buttonRestore);
    vBoxLayoutCurve->addLayout(hLayoutFilter);

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
    connect(m_listView_curve, &QListView::doubleClicked, this, &MainWnd::addCurve);
    m_model_curve = new QStringListModel(this);
    m_listView_curve->setModel(m_model_curve);
    vBoxLayoutCurve->addWidget(m_listView_curve);
    vBoxLayoutLeft->addLayout(vBoxLayoutCurve);

    initPlot();

//    QWidget *widgetRight = new QWidget;
//    widgetRight->setMinimumWidth(400);
//    widgetRight->setMinimumHeight(400);

//    QVBoxLayout *vBoxLayoutRight = new QVBoxLayout(widgetRight);
//    //    widgetRight->setLayout(vBoxLayoutRight);
//    vBoxLayoutRight->setMargin(2);
//    vBoxLayoutRight->setSpacing(5);

//    m_buttonRemoveSelectedCurve = new QPushButton("移除选中的曲线");
//    m_buttonRemoveSelectedCurve->setMinimumWidth(100);
//    m_buttonRemoveSelectedCurve->setMaximumWidth(150);
//    m_buttonRemoveSelectedCurve->setEnabled(false);
//    m_buttonRemoveAllCurves = new QPushButton("移除所有曲线");
//    m_buttonRemoveAllCurves->setMinimumWidth(100);
//    m_buttonRemoveAllCurves->setMaximumWidth(150);
//    m_buttonRemoveAllCurves->setEnabled(false);
//    m_buttonViewCurveData = new QPushButton("查看曲线数据");
//    m_buttonViewCurveData->setMinimumWidth(100);
//    m_buttonViewCurveData->setMaximumWidth(150);
//    m_buttonViewCurveData->setEnabled(false);
//    m_buttonOutputCurveData = new QPushButton("导出曲线数据");
//    m_buttonOutputCurveData->setMinimumWidth(100);
//    m_buttonOutputCurveData->setMaximumWidth(150);
//    m_buttonOutputCurveData->setEnabled(false);
//    m_buttonOutputPicture = new QPushButton("导出图形");
//    m_buttonOutputPicture->setMinimumWidth(100);
//    m_buttonOutputPicture->setMaximumWidth(150);
//    connect(m_buttonRemoveSelectedCurve, &QPushButton::clicked, m_plot, &XxwCustomPlot::removeSelectedGraph);
//    connect(m_buttonRemoveAllCurves, &QPushButton::clicked, m_plot, &XxwCustomPlot::removeAllGraphs);
//    connect(m_buttonViewCurveData, &QPushButton::clicked, m_plot, &XxwCustomPlot::viewGrpahData);
//    connect(m_buttonOutputCurveData, &QPushButton::clicked, m_plot, &XxwCustomPlot::outputGraphData);
//    connect(m_buttonOutputPicture, &QPushButton::clicked, m_plot, &XxwCustomPlot::outputPlot);
//    QHBoxLayout *hLayoutPlotFunction = new QHBoxLayout();
//    hLayoutPlotFunction->setMargin(1);
//    hLayoutPlotFunction->setSpacing(1);
//    hLayoutPlotFunction->addWidget(m_buttonRemoveSelectedCurve);
//    hLayoutPlotFunction->addWidget(m_buttonRemoveAllCurves);
//    hLayoutPlotFunction->addWidget(m_buttonViewCurveData);
//    hLayoutPlotFunction->addWidget(m_buttonOutputCurveData);
//    hLayoutPlotFunction->addWidget(m_buttonOutputPicture);
//    m_buttonRemoveSelectedCurve->hide();
//    m_buttonRemoveAllCurves->hide();
//    m_buttonViewCurveData->hide();
//    m_buttonOutputCurveData->hide();
//    m_buttonOutputPicture->hide();

//    vBoxLayoutRight->addWidget(m_plot);
//    vBoxLayoutRight->addLayout(hLayoutPlotFunction);

    QSplitter *splitter = new QSplitter;
    splitter->setOrientation(Qt::Horizontal);
    splitter->addWidget(widgetLeft);
//    splitter->addWidget(widgetRight);
    splitter->addWidget(m_plot);
    splitter->setStyleSheet("QSplitter:handle{background-color:lightgray;}");

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

    statusBar()->setVisible(true);
}


void MainWnd::initPlot()
{
    m_plot = new XxwCustomPlot();
    m_plot->showTracer(false);//不显示追踪器
    m_plot->xAxis->setLabel("时间(s)");
    m_plot->yAxis->setLabel("值");

    connect(m_plot, &XxwCustomPlot::signal_removeSelectedGraph, this, &MainWnd::removeCurve);
    connect(m_plot, &XxwCustomPlot::signal_removeAllGraphs, this, &MainWnd::removeAllCurves);

    connect(m_plot, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged()));

    // connect slot that shows a message in the status bar when a graph is clicked:
    connect(m_plot, SIGNAL(plottableClick(QCPAbstractPlottable*,int,QMouseEvent*)), this, SLOT(graphClicked(QCPAbstractPlottable*,int)));
}


void MainWnd::selectFileCur()
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
            m_comboBox_fileCurve->addItem(m_sFileCur);//新的曲线文件
        m_comboBox_fileCurve->setCurrentText(m_sFileCur);
        openFileCur();
    }
}

void MainWnd::openFileCur()
{
    //清空所有曲线
    if(m_plot)
    {
        m_plot->removeAllGraphs();
        m_plot->replot();
    }

    if(QFile::exists(m_sFileCur))
    {
        m_curMgr.m_sFileCur = m_sFileCur;
        QTimer::singleShot(0, &m_curMgr, &XCurMgr::openFile);//在子线程中读取曲线文件
        showWaitingDlg();//显示等待窗口
    }
}

void MainWnd::showWaitingDlg()
{
    if(m_pLabelWaiting)
    {
        m_pLabelWaiting->show();
        updateWaitingDlg();
    }
}

void MainWnd::updateWaitingDlg()
{
    if(m_pLabelWaiting)
    {
        m_pLabelWaiting->move((this->width()-m_pLabelWaiting->width())/2, (this->height()-m_pLabelWaiting->height())/2);
    }
}

void MainWnd::hideWaitingDlg()
{
    if(m_pLabelWaiting)
    {
        m_pLabelWaiting->hide();
    }
}

void MainWnd::updateCurveList()
{
    hideWaitingDlg();

    qDebug() << "更新曲线列表，开始...";
    m_allTypeName = m_curMgr.m_curvesMap.keys();
    m_model_curve->setStringList(m_allTypeName);
    qDebug() << "更新曲线列表，完成。";
}

void MainWnd::changeFilterKey(const QString &key)
{
    m_buttonFilter->setEnabled(!(key.trimmed().isEmpty()));
}

void MainWnd::filterCurve()
{
    qDebug() << "筛选曲线，开始...";
    m_filteredTypeName.clear();
    QString sKey = m_comboBoxFilterKey->currentText().trimmed();
    if(sKey.isEmpty())
    {
        m_filteredTypeName.append(m_allTypeName);
    }
    foreach(QString sName, m_allTypeName)
    {
        if(filterCurveByKey(sName, sKey))
            m_filteredTypeName.append(sName);
    }
    m_model_curve->setStringList(m_filteredTypeName);
    m_buttonRestore->setEnabled(true);
    m_comboBoxFilterKey->insertItem(0, sKey);
    if(m_comboBoxFilterKey->count() > 20)
        m_comboBoxFilterKey->removeItem(20);//只保留最近20个
    qDebug() << "筛选曲线，完成。";
}

void MainWnd::resotreCurve()
{
    m_model_curve->setStringList(m_allTypeName);
    m_comboBoxFilterKey->setCurrentText("");
    m_buttonFilter->setEnabled(false);
    m_buttonRestore->setEnabled(false);
}

void MainWnd::addCurveToPlot(const QString &sTypeName)
{
    if(!m_plot)
        return;

    XCurve *pCurve = Q_NULLPTR;
    if(m_curMgr.m_curvesMap.contains(sTypeName))
        pCurve = m_curMgr.m_curvesMap.value(sTypeName);
    if(pCurve)
    {
        //添加到图中
        QList<QPointF> points = pCurve->m_data;
        int nCount = points.count();
        QVector<double> xData(nCount), yData(nCount);
        for(int i = 0; i < points.count(); ++i)
        {
            if(m_curMgr.m_isBpaOut)//bpa,横轴是周波，要转换成时间，需要除以50
                xData[i] = points[i].x()/50.0f;
            else
                xData[i] = points[i].x();
            yData[i] = points[i].y();
        }
        QCPGraph *pGraph = m_plot->addGraph();
        pGraph->setData(xData, yData);
        pGraph->setName(sTypeName);
        QPen pen = pGraph->pen();
        QColor clr = getRandomColor();
        pen.setColor(clr);
        pGraph->setPen(pen);
        m_plot->rescaleAxes();
        m_plot->showScatter(m_plot->isShowScatter());
        m_plot->replot();
    }
}

void MainWnd::addCurve()
{
    if(m_plot->graphCount() >= MAX_CURVE_COUNT)
    {
        QString sMsg = QString("最多显示%1条曲线。").arg(MAX_CURVE_COUNT);
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

    if(m_selectedTypeName.contains(sTypeName))
    {
        QString sMsg = "已经选择了该条曲线，请重新选择其它曲线。";
        QMessageBox::information(this, "提示", sMsg);
        return;
    }
    m_selectedTypeName.append(sTypeName);
    addCurveToPlot(sTypeName);
//    m_buttonRemoveAllCurves->setEnabled(m_plot && m_plot->graphCount() > 0);
//    m_buttonViewCurveData->setEnabled(m_plot && m_plot->graphCount() > 0);
//    m_buttonOutputCurveData->setEnabled(m_plot && m_plot->graphCount() > 0);
}


void MainWnd::removeCurve(QString curveName)
{
    m_selectedTypeName.removeAll(curveName);
//    m_buttonRemoveSelectedCurve->setEnabled(m_plot && m_plot->selectedGraphs().size() > 0);
//    m_buttonRemoveAllCurves->setEnabled(m_plot && m_plot->graphCount() > 0);
//    m_buttonViewCurveData->setEnabled(m_plot && m_plot->graphCount() > 0);
//    m_buttonOutputCurveData->setEnabled(m_plot && m_plot->graphCount() > 0);
}

void MainWnd::removeAllCurves()
{
    m_selectedTypeName.clear();
//    m_buttonRemoveSelectedCurve->setEnabled(m_plot && m_plot->selectedGraphs().size() > 0);
//    m_buttonRemoveAllCurves->setEnabled(m_plot && m_plot->graphCount() > 0);
//    m_buttonViewCurveData->setEnabled(m_plot && m_plot->graphCount() > 0);
//    m_buttonOutputCurveData->setEnabled(m_plot && m_plot->graphCount() > 0);
}

void MainWnd::changeFileCur(const QString &file)
{
    if(m_sFileCur != file)
    {
        m_sFileCur = file;
        openFileCur();
    }
}

void MainWnd::resizeEvent(QResizeEvent *event)
{
    updateWaitingDlg();
}

// 生成一个随机颜色，为了在白色背景上显示，尽量生成深色
QColor MainWnd::getRandomColor()
{
    //方法1 使用rgb随机生成深色
    //    qsrand(QDateTime::currentMSecsSinceEpoch());
    //    int int_Red = qrand() % 256;
    //    qsrand(QDateTime::currentMSecsSinceEpoch());
    //    int int_Green = qrand() % 256;
    //    int int_Blue = (int_Red + int_Green > 400) ? 0 : 400 - int_Red - int_Green;
    //    int_Blue = (int_Blue > 255) ? 255 : int_Blue;
    //    return QColor(int_Red,int_Green,int_Blue);

    //方法2 使用hsl随机生成深色
    //HSL空间里如果L分量大于200，颜色看起来就比较淡了，所以我们可以随机生成小于200的数值作为L分量
    return QColor::fromHsl(rand()%360,rand()%256,rand()%200);
}

void MainWnd::graphClicked(QCPAbstractPlottable *plottable, int dataIndex)
{
    // since we know we only have QCPGraphs in the plot, we can immediately access interface1D()
    // usually it's better to first check whether interface1D() returns non-zero, and only then use it.
    double dataKey = plottable->interface1D()->dataMainKey(dataIndex);
    double dataValue = plottable->interface1D()->dataMainValue(dataIndex);
    QString message = QString("选中图形：'%1'，当前点：(%2, %3)")
            .arg(plottable->name()).arg(dataKey).arg(dataValue);
    statusBar()->showMessage(message, 5000);
}

void MainWnd::selectionChanged()
{
//    m_buttonRemoveSelectedCurve->setEnabled(m_plot && m_plot->selectedGraphs().size() > 0);
}

///
/// \brief filterCurveByKey:通过关键字来筛选曲线名称，
/// 即查找曲线名称中是否含有关键字，可能有多个关键字，采用逗号分割，只要包含其中一个关键字，就表示符合要求
/// \param curveName：曲线名称
/// \param key：关键字
/// \return bool:true表示含有该关键字，反之就不含该关键字
///
bool MainWnd::filterCurveByKey(const QString &curveName, const QString &key)
{
    QString sName = curveName;
    QStringList lstKeys = key.split(',');
    foreach (QString sKey, lstKeys)
    {
        if(sName.contains(sKey, Qt::CaseInsensitive))
            return true;
    }
    return false;
}
