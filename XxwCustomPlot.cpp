#include "XxwCustomPlot.h"

XxwCustomPlot::XxwCustomPlot(QWidget *parent)
    :QCustomPlot(parent)
    ,m_isShowTracer(false)
//    ,m_xTracer(Q_NULLPTR)
//    ,m_yTracer(Q_NULLPTR)
    ,m_dataTracers(QList<XxwTracer *>())
//    ,m_lineTracer(Q_NULLPTR)
{
    initPlot();
}


void XxwCustomPlot::initPlot()
{
    this->showTracer(true);//显示追踪器

    // Allow user to drag axis ranges with mouse, zoom with mouse wheel and select graphs by clicking:
    /*
 enum Interaction { iRangeDrag         = 0x001 ///< <tt>0x001</tt> Axis ranges are draggable (see \ref QCPAxisRect::setRangeDrag, \ref QCPAxisRect::setRangeDragAxes)
                   ,iRangeZoom        = 0x002 ///< <tt>0x002</tt> Axis ranges are zoomable with the mouse wheel (see \ref QCPAxisRect::setRangeZoom, \ref QCPAxisRect::setRangeZoomAxes)
                   ,iMultiSelect      = 0x004 ///< <tt>0x004</tt> The user can select multiple objects by holding the modifier set by \ref QCustomPlot::setMultiSelectModifier while clicking
                   ,iSelectPlottables = 0x008 ///< <tt>0x008</tt> Plottables are selectable (e.g. graphs, curves, bars,... see QCPAbstractPlottable)
                   ,iSelectAxes       = 0x010 ///< <tt>0x010</tt> Axes are selectable (or parts of them, see QCPAxis::setSelectableParts)
                   ,iSelectLegend     = 0x020 ///< <tt>0x020</tt> Legends are selectable (or their child items, see QCPLegend::setSelectableParts)
                   ,iSelectItems      = 0x040 ///< <tt>0x040</tt> Items are selectable (Rectangles, Arrows, Textitems, etc. see \ref QCPAbstractItem)
                   ,iSelectOther      = 0x080 ///< <tt>0x080</tt> All other objects are selectable (e.g. your own derived layerables, other layout elements,...)
                 };
     */
    this->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
                            QCP::iSelectLegend | QCP::iSelectPlottables);

    this->axisRect()->setupFullAxesBox();
    this->xAxis->setLabel("x");
    this->yAxis->setLabel("y");

    //title
    this->plotLayout()->insertRow(0);
    QCPTextElement *title = new QCPTextElement(this, "曲线图", QFont("黑体", 10, QFont::Normal));
    title->setTextColor(Qt::blue);
    this->plotLayout()->addElement(0, 0, title);

    //legend
    this->legend->setVisible(true);
    QFont legendFont = font();  // start out with XxwCustomPlot's font..
    legendFont.setPointSize(9); // and make a bit smaller for legend
    this->legend->setFont(legendFont);
    this->legend->setSelectedFont(legendFont);
    this->legend->setSelectableParts(QCPLegend::spItems); // legend box shall not be selectable, only legend items
    this->legend->setBrush(QBrush(QColor(255,255,255,230)));

    // by default, the legend is in the inset layout of the main axis rect.
    // So this is how we access it to change legend placement:
    this->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignRight);


    // connect slot that ties some axis selections together (especially opposite axes):
    connect(this, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged()));
    // connect slots that takes care that when an axis is selected, only that direction can be dragged and zoomed:
    connect(this, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePress()));
    connect(this, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel()));

    // make bottom and left axes transfer their ranges to top and right axes:
    connect(this->xAxis, SIGNAL(rangeChanged(QCPRange)), this->xAxis2, SLOT(setRange(QCPRange)));
    connect(this->yAxis, SIGNAL(rangeChanged(QCPRange)), this->yAxis2, SLOT(setRange(QCPRange)));

    // connect some interaction slots:
    connect(title, SIGNAL(doubleClicked(QMouseEvent*)), this, SLOT(titleDoubleClick(QMouseEvent*)));
    connect(this, SIGNAL(axisDoubleClick(QCPAxis*,QCPAxis::SelectablePart,QMouseEvent*)), this, SLOT(axisLabelDoubleClick(QCPAxis*,QCPAxis::SelectablePart)));
//    connect(this, SIGNAL(legendDoubleClick(QCPLegend*,QCPAbstractLegendItem*,QMouseEvent*)), this, SLOT(legendDoubleClick(QCPLegend*,QCPAbstractLegendItem*)));

    // connect slot that shows a message in the status bar when a graph is clicked:
//    connect(this, SIGNAL(plottableClick(QCPAbstractPlottable*,int,QMouseEvent*)), this, SLOT(graphClicked(QCPAbstractPlottable*,int)));

    // setup policy and connect slot for context menu popup:
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuRequest(QPoint)));
}

void XxwCustomPlot::mouseMoveEvent(QMouseEvent *event)
{
    QCustomPlot::mouseMoveEvent(event);

    if(m_isShowTracer)
    {
        //当前鼠标位置（像素坐标）
        int x_pos = event->pos().x();
        int y_pos = event->pos().y();

        //像素坐标转成实际的x,y轴的坐标
        float x_val = this->xAxis->pixelToCoord(x_pos);
        float y_val = this->yAxis->pixelToCoord(y_pos);

//        if(Q_NULLPTR == m_lineTracer)
//            m_lineTracer = new XxwTraceLine(this,XxwTraceLine::VerticalLine);//直线
//        m_lineTracer->updatePosition(x_val, y_val);

//        if(Q_NULLPTR == m_xTracer)
//            m_xTracer = new XxwTracer(this, XxwTracer::XAxisTracer);//x轴
//        m_xTracer->updatePosition(x_val, y_val);

//        if(Q_NULLPTR == m_yTracer)
//            m_yTracer = new XxwTracer(this, XxwTracer::YAxisTracer);//y轴
//        m_yTracer->updatePosition(x_val, y_val);

        int nTracerCount = m_dataTracers.count();
        int nGraphCount = graphCount();
        if(nTracerCount < nGraphCount)
        {
            for(int i = nTracerCount; i < nGraphCount; ++i)
            {
                XxwTracer *tracer = new XxwTracer(this, XxwTracer::DataTracer);
                m_dataTracers.append(tracer);
            }
        }
        else if(nTracerCount > nGraphCount)
        {
            for(int i = nGraphCount; i < nTracerCount; ++i)
            {
                XxwTracer *tracer = m_dataTracers[i];
                if(tracer)
                {
                    tracer->setVisible(false);
                }
            }
        }
        for (int i = 0; i < nGraphCount; ++i)
        {
            XxwTracer *tracer = m_dataTracers[i];
            if(!tracer)
                tracer = new XxwTracer(this, XxwTracer::DataTracer);
            tracer->setVisible(true);
            tracer->setPen(this->graph(i)->pen());
            tracer->setBrush(Qt::NoBrush);
            tracer->setLabelPen(this->graph(i)->pen());
            auto iter = this->graph(i)->data()->findBegin(x_val);
            double value = iter->mainValue();
//            double value = this->graph(i)->data()->findBegin(x_val)->value;
            tracer->updatePosition(x_val, value);
        }

        this->replot();//曲线重绘
    }
}


void XxwCustomPlot::selectionChanged()
{
    /*
   normally, axis base line, axis tick labels and axis labels are selectable separately, but we want
   the user only to be able to select the axis as a whole, so we tie the selected states of the tick labels
   and the axis base line together. However, the axis label shall be selectable individually.

   The selection state of the left and right axes shall be synchronized as well as the state of the
   bottom and top axes.

   Further, we want to synchronize the selection of the graphs with the selection state of the respective
   legend item belonging to that graph. So the user can select a graph by either clicking on the graph itself
   or on its legend item.
  */

    // make top and bottom axes be selected synchronously, and handle axis and tick labels as one selectable object:
    if (this->xAxis->selectedParts().testFlag(QCPAxis::spAxis) || this->xAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
            this->xAxis2->selectedParts().testFlag(QCPAxis::spAxis) || this->xAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
    {
        this->xAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
        this->xAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    }
    // make left and right axes be selected synchronously, and handle axis and tick labels as one selectable object:
    if (this->yAxis->selectedParts().testFlag(QCPAxis::spAxis) || this->yAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
            this->yAxis2->selectedParts().testFlag(QCPAxis::spAxis) || this->yAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
    {
        this->yAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
        this->yAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    }

    // synchronize selection of graphs with selection of corresponding legend items:
    for (int i=0; i<this->graphCount(); ++i)
    {
        QCPGraph *graph = this->graph(i);
        QCPPlottableLegendItem *item = this->legend->itemWithPlottable(graph);
        if (item->selected() || graph->selected())
        {
            item->setSelected(true);
            graph->setSelection(QCPDataSelection(graph->data()->dataRange()));
        }
    }
}

void XxwCustomPlot::mousePress()
{
    // if an axis is selected, only allow the direction of that axis to be dragged
    // if no axis is selected, both directions may be dragged
    if (this->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
        this->axisRect()->setRangeDrag(this->xAxis->orientation());
    else if (this->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
        this->axisRect()->setRangeDrag(this->yAxis->orientation());
    else
        this->axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
}

void XxwCustomPlot::mouseWheel()
{
    // if an axis is selected, only allow the direction of that axis to be zoomed
    // if no axis is selected, both directions may be zoomed
    if (this->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
        this->axisRect()->setRangeZoom(this->xAxis->orientation());
    else if (this->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
        this->axisRect()->setRangeZoom(this->yAxis->orientation());
    else
        this->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
}

//双击修改标题
void XxwCustomPlot::titleDoubleClick(QMouseEvent* event)
{
    Q_UNUSED(event)
    if (QCPTextElement *title = qobject_cast<QCPTextElement*>(sender()))
    {
        // Set the plot title by double clicking on it
        bool ok;
        QString newTitle = QInputDialog::getText(this, "QCustomPlot example", "New plot title:", QLineEdit::Normal, title->text(), &ok);
        if (ok)
        {
            title->setText(newTitle);
            this->replot();
        }
    }
}

//双击修改坐标轴标题
void XxwCustomPlot::axisLabelDoubleClick(QCPAxis *axis, QCPAxis::SelectablePart part)
{
    // Set an axis label by double clicking on it
    if (part == QCPAxis::spAxisLabel) // only react when the actual axis label is clicked, not tick label or axis backbone
    {
        bool ok;
        QString newLabel = QInputDialog::getText(this, "QCustomPlot example", "New axis label:", QLineEdit::Normal, axis->label(), &ok);
        if (ok)
        {
            axis->setLabel(newLabel);
            this->replot();
        }
    }
}

//双击修改图形（如曲线）名称
void XxwCustomPlot::legendDoubleClick(QCPLegend *legend, QCPAbstractLegendItem *item)
{
    // Rename a graph by double clicking on its legend item
    Q_UNUSED(legend)
    if (item) // only react if item was clicked (user could have clicked on border padding of legend where there is no item, then item is 0)
    {
        QCPPlottableLegendItem *plItem = qobject_cast<QCPPlottableLegendItem*>(item);
        bool ok;
        QString newName = QInputDialog::getText(this, "QCustomPlot example", "New graph name:", QLineEdit::Normal, plItem->plottable()->name(), &ok);
        if (ok)
        {
            plItem->plottable()->setName(newName);
            this->replot();
        }
    }
}

//void XxwCustomPlot::addRandomGraph()
//{
//    int n = 50; // number of points in graph
//    double xScale = (rand()/(double)RAND_MAX + 0.5)*2;
//    double yScale = (rand()/(double)RAND_MAX + 0.5)*2;
//    double xOffset = (rand()/(double)RAND_MAX - 0.5)*4;
//    double yOffset = (rand()/(double)RAND_MAX - 0.5)*10;
//    double r1 = (rand()/(double)RAND_MAX - 0.5)*2;
//    double r2 = (rand()/(double)RAND_MAX - 0.5)*2;
//    double r3 = (rand()/(double)RAND_MAX - 0.5)*2;
//    double r4 = (rand()/(double)RAND_MAX - 0.5)*2;
//    QVector<double> x(n), y(n);
//    for (int i=0; i<n; i++)
//    {
//        x[i] = (i/(double)n-0.5)*10.0*xScale + xOffset;
//        y[i] = (qSin(x[i]*r1*5)*qSin(qCos(x[i]*r2)*r4*3)+r3*qCos(qSin(x[i])*r4*2))*yScale + yOffset;
//    }

//    this->addGraph();
//    this->graph()->setName(QString("New graph %1").arg(this->graphCount()-1));
//    this->graph()->setData(x, y);
//    this->graph()->setLineStyle((QCPGraph::LineStyle)(rand()%5+1));
//    if (rand()%100 > 50)
//        this->graph()->setScatterStyle(QCPScatterStyle((QCPScatterStyle::ScatterShape)(rand()%14+1)));
//    QPen graphPen;
//    graphPen.setColor(QColor(rand()%245+10, rand()%245+10, rand()%245+10));
//    graphPen.setWidthF(rand()/(double)RAND_MAX*2+1);
//    this->graph()->setPen(graphPen);
//    this->replot();
//}

void XxwCustomPlot::removeSelectedGraph()
{
    if (this->selectedGraphs().size() > 0)
    {
        QString sName = this->selectedGraphs().first()->name();
        this->removeGraph(this->selectedGraphs().first());
        this->replot();
        emit signal_removeSelectedGraph(sName);
    }
}

void XxwCustomPlot::removeAllGraphs()
{
    this->clearGraphs();
    this->replot();
    emit signal_removeAllGraphs();
}

void XxwCustomPlot::contextMenuRequest(QPoint pos)
{
    QMenu *menu = new QMenu(this);
    menu->setAttribute(Qt::WA_DeleteOnClose);

    if (this->legend->selectTest(pos, false) >= 0) // context menu on legend requested
    {
        menu->addAction("移动到左上角", this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop|Qt::AlignLeft));
        menu->addAction("移动到上中部", this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop|Qt::AlignHCenter));
        menu->addAction("移动到右上角", this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop|Qt::AlignRight));

        menu->addSeparator();
        menu->addAction("移动到左下角", this, SLOT(moveLegend()))->setData((int)(Qt::AlignBottom|Qt::AlignLeft));
        menu->addAction("移动到下中部", this, SLOT(moveLegend()))->setData((int)(Qt::AlignBottom|Qt::AlignHCenter));
        menu->addAction("移动到右下角", this, SLOT(moveLegend()))->setData((int)(Qt::AlignBottom|Qt::AlignRight));
    }
    else  // general context menu on graphs requested
    {
//        menu->addAction("Add random graph", this, SLOT(addRandomGraph()));
        if (this->selectedGraphs().size() > 0)
            menu->addAction("删除选择的图形", this, SLOT(removeSelectedGraph()));
        if (this->graphCount() > 0)
            menu->addAction("删除所有图形", this, SLOT(removeAllGraphs()));
        menu->addSeparator();
        menu->addAction("查看数据", this, SLOT(viewGrpahData()));
        menu->addSeparator();
        menu->addAction("导出图形", this, SLOT(outputPlot()));
        menu->addSeparator();
        QAction *action = menu->addAction("跟随鼠标显示曲线上的值", this, SLOT(showValueAtMousePoint()));
        action->setCheckable(true);
        action->setChecked(m_isShowTracer);
    }
    menu->popup(this->mapToGlobal(pos));
}

void XxwCustomPlot::moveLegend()
{
    if (QAction* contextAction = qobject_cast<QAction*>(sender())) // make sure this slot is really called by a context menu action, so it carries the data we need
    {
        bool ok;
        int dataInt = contextAction->data().toInt(&ok);
        if (ok)
        {
            this->axisRect()->insetLayout()->setInsetAlignment(0, (Qt::Alignment)dataInt);
            this->replot();
        }
    }
}

//查看图形数据
void XxwCustomPlot::viewGrpahData()
{
    QString sMsg = "开发中...";
    QMessageBox::information(this, "提示", sMsg);
}

//导出图形
void XxwCustomPlot::outputPlot()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    "保存图形文件",
                                                    qApp->applicationDirPath(),
                                                    "图形文件(*.png *.jpg *.bmp) ;; PDF文件(*.pdf)");
    QFileInfo fi(fileName);
    QString sSuffix = fi.suffix().toLower();
    if("png" == sSuffix)
        this->savePng(fileName);
    if("jpg" == sSuffix)
        this->saveJpg(fileName);
    if("bmp" == sSuffix)
        this->saveBmp(fileName);
    if("pdf" == sSuffix)
        this->savePdf(fileName);
}

void XxwCustomPlot::showValueAtMousePoint()
{
    if (QAction* contextAction = qobject_cast<QAction*>(sender())) // make sure this slot is really called by a context menu action, so it carries the data we need
    {
        showTracer(contextAction->isChecked());
        this->replot();
    }
}
