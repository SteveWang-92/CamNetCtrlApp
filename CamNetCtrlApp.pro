QT       += core gui network sql charts
#设置程序的图标
RC_ICONS = myicon.ico

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    caminfodb.cpp \
    historyrecord.cpp \
    hostip.cpp \
    linechart.cpp \
    main.cpp \
    camnetctrlwidget.cpp \
    mousemovegraphicsview.cpp \
    netsocketmodel.cpp \
    recordsavemodel.cpp \
    singledeviceinfo.cpp \
    tabmodelalicenter.cpp \
    zoomchart.cpp \
    zoomchartview.cpp

HEADERS += \
    caminfodb.h \
    camnetctrlwidget.h \
    historyrecord.h \
    hostip.h \
    linechart.h \
    mousemovegraphicsview.h \
    netsocketmodel.h \
    recordsavemodel.h \
    singledeviceinfo.h \
    tabmodelalicenter.h \
    zoomchart.h \
    zoomchartview.h

FORMS += \
    camnetctrlwidget.ui \
    historyrecord.ui \
    singledeviceinfo.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
