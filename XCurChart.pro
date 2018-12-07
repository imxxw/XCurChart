#-------------------------------------------------
#
# Project created by QtCreator 2018-12-06T20:13:37
#
#-------------------------------------------------

QT       += core gui charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = XCurChart
TEMPLATE = app
RC_FILE += Res/version.rc

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        MainWindow.cpp \
#    XxwCppPub/Curve.cpp \
#    XxwCppPub/XxwCppPub.cpp \
    XCurMgr.cpp

HEADERS += \
        MainWindow.h \
#    XxwCppPub/Curve.h \
#    XxwCppPub/XxwCppPub.h \
    XCurMgr.h \
    Res/version.rc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


CONFIG(debug, debug|release) {
  #设置debug配置下编译生成文件的路径
  TARGET = $$join(TARGET,,,d)   #为debug版本生成的文件增加d的后缀
  contains(TEMPLATE, "lib") {
    DESTDIR = $$PWD/Bin/        #将库放在当前工程的Bin/文件夹下
    DLLDESTDIR = $$PWD/Bin/    #将动态库放在当前工程的Bin/文件夹下
  } else {
    DESTDIR = $$PWD/Bin//        #将应用程序放在当前工程的Bin/文件夹下
  }
  OBJECTS_DIR = ./debug/obj     #将生成的对象文件放在专门的obj文件夹下
  MOC_DIR = ./debug/moc         #将QT自动生成的对象放在moc文件夹下
} else {
  #设置release配置下编译生成文件的路径
  contains(TEMPLATE, "lib") {
    DESTDIR = $$PWD/Bin/       #将库放在当前工程的Bin/文件夹下
    DLLDESTDIR = $$PWD/Bin/    #将动态库放在当前工程的Bin/文件夹下
  } else {
    DESTDIR = $$PWD/Bin/       #将应用程序放在当前工程的Bin/文件夹下
  }
  OBJECTS_DIR = ./release/obj   #将生成的对象文件放在专门的obj文件夹下
  MOC_DIR = ./release/moc       #将QT自动生成的对象放在moc文件夹下
}

RESOURCES += \
    Res.qrc
