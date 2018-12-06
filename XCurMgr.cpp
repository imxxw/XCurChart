#include "XCurMgr.h"
#include <QDebug>
#include <QFile>
#include <QDataStream>

const int c_nAccuacyOfTime = 2;//时间值精确到小数点后的位数
const int c_nAccuacyOfCurveValue = 6;//曲线上的值，精确到小数点后的位数
const int c_nTag = 960829;

const int size_char = sizeof(char);
const int size_int = sizeof(int);
const int size_float = sizeof(float);
const int size_double = sizeof(double);

XCurMgr::XCurMgr()
{
    m_isBpaOut = false;
    this->moveToThread(&m_thread);
    m_thread.start();
    qDebug() << "XCurMgr::XCurMgr thread: " << QThread::currentThread();
}

void XCurMgr::releaseAll()
{
    if(m_allCurves.isEmpty())
        return;
    QMap<QString, XCurve *>::iterator it = m_allCurves.begin();
    for (; it != m_allCurves.end(); ++it)
    {
        XCurve *pCurve = it.value();
        if(pCurve)
        {
            delete pCurve;
            pCurve = Q_NULLPTR;
        }
    }
    m_allCurves.clear();
    m_isBpaOut = false;
}


void XCurMgr::openFile()
{
    //    m_curMgr.SetFilePath(m_sFileCur.toStdString());
    //    m_curMgr.OpenFile();
    //    buildCurves();

    //读二进制文件
    int m_nTag;//tag（cur文件的标志，常量：960829，如果不是960829,就表示不是正确的cur文件）
    int m_nSampleCount;//采样点的总数，同一个曲线文件中所有的曲线的采样点数都一样
    QList<float> m_Times;//时间信息（实际上，采样间隔前后不一定相同）
    QStringList m_Comments;//描述信息
    QList<XCurve *> m_Curves;

    QFile file(m_sFileCur);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "无法打开曲线文件:" << m_sFileCur;
        return;
    }
    QDataStream fin(&file);
    //    fin.setVersion(QDataStream::Qt_5_7);

    qDebug()  << "文件：" << m_sFileCur << " 读取中...\n";
    bool bConsole = false;//是否输出到控制台
    if (bConsole)
        qDebug()  << "原始数据：" << "\n";

    //读取标识信息
    fin.readRawData((char *)&m_nTag, size_int);
    if (m_nTag != c_nTag)
    {
        qDebug()  << "cur文件格式不对。";
        file.close();
        return ;
    }
    if (bConsole)
        qDebug()  << "标识：" << m_nTag << "\n";

    //读取曲线数
    int nCurves = 0;
    fin.readRawData((char *)&nCurves, size_int);
    if (nCurves > 0)
        m_Curves.reserve(nCurves);//一次性分配好大小，效率更高，速度更快
    if (bConsole)
        qDebug()  << "曲线数：" << nCurves << "\n";

    //读取采样数（也就是每条曲线上的点的数目）
    fin.readRawData((char *)&m_nSampleCount, size_int);
    if (m_nSampleCount > 0)
        m_Times.reserve(m_nSampleCount);//一次性分配好大小，效率更高，速度更快
    if (bConsole)
        qDebug()  << "总采样数（每条曲线上的点数）：" << m_nSampleCount << "\n";

    //读解释信息总数
    int nComments = 0;
    fin.readRawData((char *)&nComments, size_int);
    if (nComments)
        m_Comments.reserve(nComments);//一次性分配好大小，效率更高，速度更快
    if (bConsole)
        qDebug()  << "解释数目：" << nComments << "\n";

    //解释信息（一条接一条信息，每条信息80个字节长）
    for (int i = 0; i < nComments; ++i)
    {
        char sCom[80] = "";
        fin.readRawData(sCom, 80);
        sCom[79] = 0;
        m_Comments.append(sCom);
        if (bConsole)
            qDebug()  << "解释" << i << "：" << sCom << "\n";
    }

    //曲线信息：一共nCurves条曲线，每条曲线包含：名称和类型两个字符串
    //名称占80个字节，类型占20个字节
    if (bConsole)
        qDebug()  << "曲线信息：" << "\n";
    for (int i = 0; i < nCurves; ++i)
    {
        char sName[80] = "";//名称
        fin.readRawData(sName, 80);
        sName[79] = 0;

        if(0 == i)
        {
            QString curve_name = sName;
            curve_name = curve_name.trimmed();
            //第一条曲线是"步长h(sec.)"，则不是BPA输出的曲线
            //否则就是BPA输出的曲线
//            m_isBpaOut = !(0 == curve_name.compare("步长h(sec.)",Qt::CaseInsensitive));
            m_isBpaOut = !curve_name.startsWith("步长");
        }

        char sType[20] = "";//单位（类型）
        fin.readRawData(sType, 20);
        sType[19] = 0;
        if (bConsole)
            qDebug()  << "曲线" << i << "：" << sName << ", " << sType << "\n";
        XCurve *newCurve = new XCurve;
        newCurve->m_sName = sName;
        newCurve->m_sType = sType;
        newCurve->m_data.reserve(m_nSampleCount);//一次性分配好大小，效率更高，速度更快
        m_Curves.append(newCurve);
        QString sTypeName = newCurve->m_sType + "-" + newCurve->m_sName;
        m_allCurves.insert(sTypeName, newCurve);
    }

    //时间信息，一共m_nSampleCount个采样点
    if (bConsole)
        qDebug()  << "采样时间：" << "\n";
    for (int i = 0; i < m_nSampleCount; ++i)
    {
        float fTime = 0;
        fin.readRawData((char *)&fTime, size_float);
        //20180202 xiongxuanwen
        //时间精确到小数点后面2位，也就是0.01
//        fTime = CXxwCppPub::GetAccuracy(fTime, c_nAccuacyOfTime);
        m_Times.append(fTime);
        if (bConsole)
        {
            qDebug()  << " " << fTime;
            if ((i + 1) % 10 == 0)
                qDebug()  << "\n";//换行
        }
    }
    if (bConsole)
        qDebug()  << "\n";

    //曲线上采样点的值（一共nCurves条曲线，每条曲线含m_nSampleCount个值）
    if (bConsole)
        qDebug()  << "曲线数据：" << "\n";
    for (int i = 0; i < nCurves; ++i)
    {
        if (bConsole)
            qDebug()  << "曲线" << i << "的点数据：" << "\n";
        XCurve * pCurve = m_Curves.at(i);
        if (!pCurve)
            continue;
        for (int j = 0; j < m_nSampleCount; ++j)
        {
            float fValue = 0;
            fin.readRawData((char *)&fValue, size_float);
            //20180202 xiongxuanwen
            //精确到小数点后面4位，也就是0.0001
//            fValue = CXxwCppPub::GetAccuracy(fValue, c_nAccuacyOfCurveValue);
            QPointF pt(m_Times.at(j), fValue);
            pCurve->m_data.append(pt);
            if (bConsole)
            {
                qDebug()  << " " << fValue;
                if ((j + 1) % 10 == 0)
                    qDebug()  << "\n";//换行
            }
        }
        if (bConsole)
            qDebug()  << "\n";
    }
    file.close();

    if (m_Curves.count() > 0)
    {
        XCurve *pCurve = m_Curves.at(0);
        QString curve_name = pCurve->m_sName;
        curve_name = curve_name.trimmed();
        //第一条曲线是"步长h(sec.)"，则不是BPA输出的曲线
        //否则就是BPA输出的曲线
        m_isBpaOut = !(0 == curve_name.compare("步长h(sec.)",Qt::CaseInsensitive));
    }

    qDebug()  << "文件：" << m_sFileCur << " 读取完毕。\n";
    qDebug()  << "解释数目：" << m_Comments.count() << "\n";
    qDebug()  << "曲线数目：" << m_Curves.count() << "\n";
    qDebug()  << "采样点数：" << m_nSampleCount << "\n";
    emit signal_DataReady();//数据已经准备好了
}
