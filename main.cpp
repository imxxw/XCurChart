#include "MainWindow.h"
#include <QApplication>
#include <QStringList>
#include <QFileInfo>
#include <QDebug>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QString sFileCur = "";//曲线文件
    // 使用QApplication::arguments()获取命令行参数，无需转换（其实qt内部已经将命令行参数进行了转换）
    QStringList lstArgs = QApplication::arguments();
    if(lstArgs.count() > 1)
    {
        QString sFileTemp = lstArgs.at(1);
        qDebug() << "命令行参数:" << sFileTemp;
        QFileInfo fi(sFileTemp);
        if("cur" == fi.suffix().toLower())
            sFileCur = sFileTemp;//输入文件
    }

    if (!sFileCur.isEmpty() && !QFile::exists(sFileCur))
    {
        QString sMsg = QString("曲线文件——%1不存在。").arg(sFileCur);
        QMessageBox::warning(Q_NULLPTR,"错误",sMsg);
        sFileCur = "";
    }

    MainWindow w(sFileCur);
    w.show();

    return a.exec();
}
