#-------------------------------------------------
#
# Project created by QtCreator 2014-05-20T12:37:25
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = untitled
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    dialog.cpp

HEADERS  += mainwindow.h \
    dialog.h

FORMS    += mainwindow.ui \
    dialog.ui

RESOURCES += \
    newResource.qrc

INCLUDEPATH += C:\opencv\build\include \

CONFIG(release,debug|release)
{
    LIBS += C:\opencv\build\x86\vc10\lib\opencv_calib3d248.lib \
            C:\opencv\build\x86\vc10\lib\opencv_contrib248.lib \
            C:\opencv\build\x86\vc10\lib\opencv_core248.lib \
            C:\opencv\build\x86\vc10\lib\opencv_features2d248.lib \
            C:\opencv\build\x86\vc10\lib\opencv_flann248.lib \
            C:\opencv\build\x86\vc10\lib\opencv_gpu248.lib \
            C:\opencv\build\x86\vc10\lib\opencv_highgui248.lib \
            C:\opencv\build\x86\vc10\lib\opencv_imgproc248.lib \
            C:\opencv\build\x86\vc10\lib\opencv_legacy248.lib \
            C:\opencv\build\x86\vc10\lib\opencv_ml248.lib \
            C:\opencv\build\x86\vc10\lib\opencv_ts248.lib \
            C:\opencv\build\x86\vc10\lib\opencv_video248.lib \
            C:\opencv\build\x86\vc10\lib\opencv_objdetect248.lib \
            C:\opencv\build\x86\vc10\lib\opencv_gpu248.lib \
            C:\opencv\build\x86\vc10\lib\opencv_nonfree248.lib
}

CONFIG(debug,debug|release)
{
    LIBS += C:\opencv\build\x86\vc10\lib\opencv_calib3d248d.lib \
            C:\opencv\build\x86\vc10\lib\opencv_contrib248d.lib \
            C:\opencv\build\x86\vc10\lib\opencv_core248d.lib \
            C:\opencv\build\x86\vc10\lib\opencv_features2d248d.lib \
            C:\opencv\build\x86\vc10\lib\opencv_flann248d.lib \
            C:\opencv\build\x86\vc10\lib\opencv_gpu248d.lib \
            C:\opencv\build\x86\vc10\lib\opencv_highgui248d.lib \
            C:\opencv\build\x86\vc10\lib\opencv_imgproc248d.lib \
            C:\opencv\build\x86\vc10\lib\opencv_legacy248d.lib \
            C:\opencv\build\x86\vc10\lib\opencv_ml248d.lib \
            C:\opencv\build\x86\vc10\lib\opencv_ts248d.lib \
            C:\opencv\build\x86\vc10\lib\opencv_video248d.lib \
            C:\opencv\build\x86\vc10\lib\opencv_objdetect248d.lib \
            C:\opencv\build\x86\vc10\lib\opencv_gpu248d.lib \
            C:\opencv\build\x86\vc10\lib\opencv_nonfree248d.lib
}

