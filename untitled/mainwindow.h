#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <Qmessagebox>
#include <QFileDialog>
#include <QLabel>
#include <QMouseEvent>
#include <iostream>
#include <qdebug>
#include <stdio.h>
#include <QPainter>
#include <qpicture>
#include <QPolygon>
#include <QtEndian>
#include <opencv/cv.h>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/features2d/features2d.hpp>
#include "opencv2/nonfree/features2d.hpp"
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <QProcess>
#include <QMovie>

#include <cstdlib>
#include <opencv/highgui.h>
#include "dialog.h"

using namespace std;
using namespace cv;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void drawImage(IplImage* target, IplImage* source, int x, int y, IplImage* source2, IplImage* source3);
    void setMax();
    void setWaitScreen();
    void autoDrawing();
    ~MainWindow();


private slots:
    void mousePressEvent(QMouseEvent *event);
    void on_slideNOK_clicked();
    void on_backButton_clicked();
    void on_frontButton_clicked();
    void on_backButton2_clicked();
    void on_frontButton2_clicked();
    void on_slideOK_clicked();
    void on_nokSlide1_clicked();
    void on_manual_clicked();
    void on_okSlide1_clicked();
    void on_okSlide2_clicked();
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_backButton2_2_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

private:
    Ui::MainWindow *ui;
    QString slide_filename;
    QString video_filename;
    QString video_filename2;

    int idx;
    QImage source, cutout;
    float width, height;
    QPixmap buf;
    QPixmap buf2;
    QString filename;
    int x1, x2, x3, x4, y1, y2, y3, y4;

    CvCapture* capture, *capture2;
    cv::Mat firstFrame;
    IplImage* frame;
    double nFps;

    int time;
    int pptTime[100];   // have ppt slide time

    QPixmap tempBuf;
    int slideNum;
    int lengthFrame;
    QString fn1, fn2;

    int rightX, leftX, upY, downY;

    Dialog *a;
    Dialog *b;

    int isAuto;
    int autoIdx;
    int autoX1[3], autoY1[3], autoX2[3], autoY2[3], autoX3[3], autoY3[3], autoX4[3], autoY4[3];
};

#endif // MAINWINDOW_H
