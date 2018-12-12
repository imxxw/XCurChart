#ifndef CURVEDATATABLEMODEL_H
#define CURVEDATATABLEMODEL_H

#include "qcustomplot.h"
#include <QAbstractTableModel>
#include <QList>

///
/// \brief 自定义的子任务表格模型
///
class CurveDataTableModel:public QAbstractTableModel
{
    Q_OBJECT

public:
    CurveDataTableModel(QObject *parent = nullptr);
    ~CurveDataTableModel();

    // 行数
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    // 列数
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    // 表头数据
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;

//    // 设置表格项数据，根据角色（颜色、文本、对齐方式、选中状态等）判断需要设置的内容。
//    bool setData(const QModelIndex &index, const QVariant &value, int role);

    // 表格项数据
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    //获取制定单元格的显示字符串
    QString itemShowText(int nRow,int nColumn);

    // 用来设置单元格的标志（可用、可选中、可复选等）。
//    Qt::ItemFlags flags(const QModelIndex &index) const;

    // 更新表格数据
    void setModelData(QCPGraph *modelData)
    {
        m_modelData = modelData;
        beginResetModel();
        endResetModel();
    }

    //获取表格数据
    QCPGraph * modelData()
    {
        return m_modelData;
    }

public slots:
    // 更新表格(无需更新数据)
    void updateModel()
    {
        beginResetModel();
        endResetModel();
    }

protected:
    QCPGraph *m_modelData;//表格模型中的数据
};

#endif // CURVEDATATABLEMODEL_H
