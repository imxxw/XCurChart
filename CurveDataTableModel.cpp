#include "CurveDataTableModel.h"
#include <QColor>

#define COLUMN_X 0
#define COLUMN_Y 1

CurveDataTableModel::CurveDataTableModel(QObject *parent)
    : QAbstractTableModel(parent),m_modelData(Q_NULLPTR)
{
}

CurveDataTableModel::~CurveDataTableModel()
{
}


// 行数
int CurveDataTableModel::rowCount(const QModelIndex &parent) const
{
    if(m_modelData)
        return m_modelData->dataCount();
    else
        return 0;
}

// 列数
int CurveDataTableModel::columnCount(const QModelIndex &parent) const
{
    return 2;
}

// 表头数据
QVariant CurveDataTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch (role)
    {
    case Qt::TextAlignmentRole:
        return QVariant(Qt::AlignVCenter | Qt::AlignHCenter);
    case Qt::DisplayRole:
    {
        if (orientation == Qt::Horizontal)
        {
            if (COLUMN_X == section)
                return "时间(s)";
            else if (COLUMN_Y == section)
                return "值";
        }
        else //垂直列头，显示行号，行号从1开始
            return section + 1;//行号从1开始
    }
    default:
        return QVariant();
    }

    return QVariant();
}

// 设置表格项数据，根据角色（颜色、文本、对齐方式、选中状态等）判断需要设置的内容。
//bool CurveDataTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
//{
//    if (!index.isValid())
//        return false;

//    int nRow = index.row();
//    int nColumn = index.column();

//    return false;
//}

// 表格项数据,用来显示数据，根据角色（颜色、文本、对齐方式、选中状态等）判断需要显示的内容
QVariant CurveDataTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    int nRow = index.row();
    int nColumn = index.column();    
    if(nRow < 0 || nRow >= rowCount())
        return QVariant();
    if(nColumn < 0 || nColumn >= columnCount())
        return QVariant();

    switch (role)
    {
    case Qt::TextAlignmentRole:
        return QVariant(Qt::AlignHCenter | Qt::AlignVCenter);
    case Qt::DisplayRole:
    {
        double key = m_modelData->data()->at(nRow)->mainKey();
        double val = m_modelData->data()->at(nRow)->mainValue();
        if (COLUMN_X == nColumn)
            return QString::number(key);
        else if (COLUMN_Y == nColumn)
            return QString::number(val);
        return "";
    }
    default:
        return QVariant();
    }
    return QVariant();
}

//获取制定单元格的显示字符串
QString CurveDataTableModel::itemShowText(int nRow,int nColumn)
{
    if(nRow < 0 || nRow >= rowCount())
        return "";
    if(nColumn < 0 || nColumn >= columnCount())
        return "";
    double key = m_modelData->data()->at(nRow)->mainKey();
    double val = m_modelData->data()->at(nRow)->mainValue();
    if (COLUMN_X == nColumn)
        return QString::number(key);
    else if (COLUMN_Y == nColumn)
        return QString::number(val);
    return "";
}

// 表格可选中、可复选
//Qt::ItemFlags CurveDataTableModel::flags(const QModelIndex &index) const
//{
//    Qt::ItemFlags flags = QAbstractItemModel::flags(index);
//    if (!index.isValid())
//      return flags;

//    int row_num = index.row();
//    int col_num = index.column();
//    SubTask *task = m_modelData.at(row_num);
//    if(nullptr == task)
//        return flags;

//    if (index.column() == COLUMN_X)//对于第一列设置标志位
//    {
//        if(SubTask::Status_Success == task->m_status)
//            return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
//        else
//            return Qt::ItemIsUserCheckable;//没有计算完成，勾选框被禁用
//    }
//    return flags;
//}
