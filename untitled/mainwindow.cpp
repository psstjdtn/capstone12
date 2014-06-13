#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->slideOK->setVisible(true);
    ui->slideNOK->setVisible(true);
    ui->manual->setVisible(false);  // 이거 true 로 바꿔라

    ui->okSlide1->setVisible(false);
    ui->okSlide2->setVisible(false);
    ui->nokSlide1->setVisible(false);

    ui->backButton->setVisible(false);
    ui->frontButton->setVisible(false);
    ui->backButton2->setVisible(false);
    ui->frontButton2->setVisible(false);
    ui->label->setVisible(false);

    ui->pushButton->setVisible(false);
    ui->pushButton_2->setVisible(false);
    ui->pushButton_3->setVisible(false);
    ui->pushButton_4->setVisible(false);
    ui->pushButton_5->setVisible(false);
    ui->pushButton_6->setVisible(false);

    slide_filename = "";
    video_filename = "";
    video_filename2 = "";

    resize(1585, 830);

    idx = 2;
    time = 0;
    isAuto = 0;
    autoIdx = 0;

    for(int i=0; i<100; i++)
        pptTime[i] = 0;
}

MainWindow::~MainWindow()
{
    delete ui;
}

static const double pi = 3.14159265358979323846;
static const int MAX_SIZE = 100;

inline static double square(int a)
{
    return a * a;
}

inline static void allocateOnDemand( IplImage **img, CvSize size, int depth, int channels )
{
    if ( *img != NULL )   return;

    *img = cvCreateImage( size, depth, channels );
    if ( *img == NULL )
    {
        fprintf(stderr, "Error: Couldn't allocate image.  Out of memory?\n");
        exit(-1);
    }
}

QImage Mat2QImage(cv::Mat const& src)
{
    cv::Mat temp; // make the same cv::Mat
    cvtColor(src, temp,CV_BGR2RGB); // cvtColor Makes a copt, that what i need
    QImage dest((uchar*) temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
    QImage dest2(dest);
    dest2.detach(); // enforce deep copy
    return dest2;
}

cv::Mat QImage2Mat(QImage const& src)
{
    cv::Mat tmp(src.height(),src.width(),CV_8UC3,(uchar*)src.bits(),src.bytesPerLine());
    cv::Mat result; // deep copy just in case (my lack of knowledge with open cv)
    cvtColor(tmp, result,CV_BGR2RGB);
    return result;
}

IplImage* QImage2IplImage(QImage *qimg)
{

    IplImage *imgHeader = cvCreateImageHeader( cvSize(qimg->width(), qimg->height()), IPL_DEPTH_8U, 4);
    imgHeader->imageData = (char*) qimg->bits();

    uchar* newdata = (uchar*) malloc(sizeof(uchar) * qimg->byteCount());
    memcpy(newdata, qimg->bits(), qimg->byteCount());
    imgHeader->imageData = (char*) newdata;
    //cvClo
    return imgHeader;
}

void MainWindow::setMax()
{
    rightX = x1;
    if( rightX < x2) rightX = x2;
    if( rightX < x3) rightX = x3;
    if( rightX < x4) rightX = x4;

    leftX = x1;
    if( leftX > x2) leftX = x2;
    if( leftX > x3) leftX = x3;
    if( leftX > x4) leftX = x4;

    upY = y1;
    if( upY > y2) upY = y2;
    if( upY > y3) upY = y3;
    if( upY > y4) upY = y4;

    downY = y1;
    if( downY < y2) downY = y2;
    if( downY < y3) downY = y3;
    if( downY < y4) downY = y4;

    int ruX, ruY, rdX, rdY, luX, luY, ldX, ldY;

    if( x1 < x3 )
    {
        if( y1 < y3 )
        {
            if( x2 > x4 )
            {
                luX = x1; luY = y1; ruX = x2; ruY = y2;
                ldX = x4; ldY = y4; rdX = x3; rdY = y3;
            }
            else
            {
                luX = x1; luY = y1; ruX = x4; ruY = y4;
                ldX = x2; ldY = y2; rdX = x3; rdY = y3;
            }
        }

        else    // y3 < y1
        {
            if( x2 > x4 )
            {
                luX = x4; luY = y4; ruX = x3; ruY = y3;
                ldX = x1; ldY = y1; rdX = x2; rdY = y2;
            }

            else
            {
                luX = x2; luY = y2; ruX = x3; ruY = y3;
                ldX = x1; ldY = y1; rdX = x4; rdY = y4;
            }
        }
    }

    else
    {
        if( y1 < y3 )
        {
            if( x2 > x4 )
            {
                luX = x4; luY = y4; ruX = x1; ruY = y1;
                ldX = x3; ldY = y3; rdX = x2; rdY = y2;
            }
            else
            {
                luX = x2; luY = y2; ruX = x1; ruY = y1;
                ldX = x3; ldY = y3; rdX = x4; rdY = y4;
            }
        }

        else    // y3 < y1
        {
            if( x2 > x4 )
            {
                luX = x3; luY = y3; ruX = x2; ruY = y2;
                ldX = x4; ldY = y4; rdX = x1; rdY = y1;
            }

            else
            {
                luX = x3; luY = y3; ruX = x4; ruY = y4;
                ldX = x2; ldY = y2; rdX = x1; rdY = y1;
            }
        }
    }

    x1 = luX; y1 = luY; x2 = ruX; y2 = ruY;
    x4 = ldX; y4 = ldY; x3 = rdX; y3 = rdY;
}

String pHashValue(Mat &src)
{
    Mat img ,dst;
    String rst(65536,'\0');

    double dIdex[65536];
    double mean = 0.0;
    int k = 0;
    if(src.channels()==3)
    {
        cvtColor(src,src,CV_BGR2GRAY);
        img = Mat_<double>(src);
    }
    else
    {
        img = Mat_<double>(src);
    }

    resize(img, img, Size(256,256));
    dct(img, dst);

    for (int i = 0; i < 256; ++i)
    {
        for (int j = 0; j < 256; ++j)
        {
            dIdex[k] = dst.at<double>(i, j);
            mean += dst.at<double>(i, j)/65536;
            ++k;
        }
    }

    for (int i =0;i<65536;++i)
    {
        if( i > 57856 )
        {
            rst[i] = '1';
        }

        else
        {
            if (dIdex[i]>=mean)
            {
                rst[i]='1';
            }
            else
            {
                rst[i]='0';
            }
        }
    }

    return rst;
}

int HammingDistance(String &str1, String &str2)
{
    if((str1.size()!=65536)||(str2.size()!=65536))
        return -1;
    int difference = 0;
    for(int i=0;i<65536;i++)
    {
        if(str1[i]!=str2[i])
            difference++;
    }
    return difference;
}

void MainWindow::drawImage(IplImage* target, IplImage* source, int x, int y, IplImage* source2, IplImage* source3) // xframe, cframe
{
    int r, g, b;
    int r1, g1, b1;

    CvScalar bgr;
    //source->width, height
    for (int ix=0; ix<source->width; ix++) {

        if( ix < leftX ){ ix = leftX-1; continue; }
        if( ix > rightX ) break;

        for (int iy=0; iy<source->height; iy++) {

            if( iy < upY ){ iy = upY-1; continue; }
            if( iy > downY ){ break; }

            r = cvGet2D(source, iy, ix).val[2];
            g = cvGet2D(source, iy, ix).val[1];
            b = cvGet2D(source, iy, ix).val[0];

            if( r == 0 && g == 0 && b == 0 ) continue;

            bgr = cvScalar(b, g, r);
            cvSet2D(target, iy, ix, bgr);

            /* 시작 */
            r = cvGet2D(source2, iy, ix).val[2];
            g = cvGet2D(source2, iy, ix).val[1];
            b = cvGet2D(source2, iy, ix).val[0];

            r1 = cvGet2D(source3, iy, ix).val[2];
            g1 = cvGet2D(source3, iy, ix).val[1];
            b1 = cvGet2D(source3, iy, ix).val[0];

            bgr = cvScalar(b1, g1, r1);

            //            if( x == 0 )    // strong
            //            {
            //                if( abs(r-r1) > 40 || abs(g-g1) > 40 || abs(b-b1) > 40 ) cvSet2D(target, iy, ix, bgr);
            //            }
            //            else            // weak
            //            {
            //                if( abs(r-r1) > 80 || abs(g-g1) > 80 || abs(b-b1) > 80 ) cvSet2D(target, iy, ix, bgr);
            //            }

            if( abs(r-r1) > 35 || abs(g-g1) > 35 || abs(b-b1) > 35 ) cvSet2D(target, iy, ix, bgr);
        }
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    qDebug("main clicked!");

    if( isAuto == 1 )
    {
        //buf2 = QPixmap::fromImage(source);

        QString temp;
        QString temp2;

        temp = temp.setNum(event->pos().x());
        temp2 = temp2.setNum(event->pos().y());

        QPainter p(&buf2);
        p.setPen(Qt::red);

        QPen linepen(Qt::red);
        linepen.setCapStyle(Qt::RoundCap);
        linepen.setWidth(5);

        p.setPen(linepen);

        if( idx == 2)
        {
            x1 = event->pos().x(); y1 = event->pos().y();
            p.drawPoint(x1-10, y1-20);

            ui->label->setPixmap(buf);
        }

        else if( idx == 3)
        {
            x2 = event->pos().x(); y2 = event->pos().y();
            p.drawLine(x1-10, y1-20, x2-10, y2-20);

        }
        else if( idx == 4)
        {
            x3 = event->pos().x(); y3 = event->pos().y();
            p.drawLine(x2-10, y2-20, x3-10, y3-20);

        }
        else if( idx == 5)
        {
            x4 = event->pos().x(); y4 = event->pos().y();
            p.drawLine(x3-10, y3-20, x4-10, y4-20);
            p.drawLine(x1-10, y1-20, x4-10, y4-20);
        }

        idx++;
        p.end();
        ui->label->setPixmap(buf2);

        if( idx == 6 )
        {
            buf2 = QPixmap::fromImage(source);

            QPolygon region;
            region << QPoint(x1-10, y1-20) << QPoint(x2-10, y2-20) << QPoint(x3-10, y3-20) << QPoint(x4-10, y4-20);

            QPainterPath path;
            path.addPolygon(region);

            cutout = QImage(source.size(), QImage::Format_ARGB32_Premultiplied);
            //cutout.fill(Qt::transparent);

            cutout.fill(QColor(Qt::black).rgb());

            QPainter p(&cutout);
            p.setClipPath(path);
            p.drawImage(0, 0, source);
            p.end();

            cutout.save("test.jpg");

            idx = 2;

            x1 = (x1-10);
            x2 = (x2-10);
            x3 = (x3-10);
            x4 = (x4-10);
            y1 = (y1-20);
            y2 = (y2-20);
            y3 = (y3-20);
            y4 = (y4-20);

            setMax();
        }
    }
}

void MainWindow::on_slideNOK_clicked()
{
    ui->slideOK->setVisible(false);
    ui->slideNOK->setVisible(false);
    ui->manual->setVisible(false);

    ui->okSlide1->setVisible(false);
    ui->okSlide2->setVisible(false);
    ui->nokSlide1->setVisible(true);

    ui->backButton->setVisible(false);
    ui->frontButton->setVisible(false);
    ui->backButton2->setVisible(true);
    ui->frontButton2->setVisible(true);


}


void MainWindow::on_backButton_clicked()
{
    ui->slideOK->setVisible(true);
    ui->slideNOK->setVisible(true);
    ui->manual->setVisible(false);

    ui->okSlide1->setVisible(false);
    ui->okSlide2->setVisible(false);
    ui->nokSlide1->setVisible(false);

    ui->backButton->setVisible(false);
    ui->frontButton->setVisible(false);
    ui->backButton2->setVisible(false);
    ui->frontButton2->setVisible(false);

    slide_filename = "";
    video_filename = "";

    ui->frontButton->setStyleSheet("border-image: url(:/8.jpg);");

}

void MainWindow::on_frontButton_clicked()   // '슬라이드 있다'의 최종 단계 버튼
{
    //MainWindow::on_pushButton_3_clicked();

    if( video_filename != "" && slide_filename != "")
    {

        b = new Dialog();

        b->show();

        QApplication::processEvents();


        qDebug("front Button clicked!!");

        ui->label->setVisible(true);
        ui->okSlide1->setVisible(false);
        ui->okSlide2->setVisible(false);
        ui->backButton->setVisible(false);
        ui->frontButton->setVisible(false);
        ui->pushButton->setVisible(true);
        ui->pushButton_4->setVisible(true);
        ui->pushButton_5->setVisible(true);

        QByteArray bytename = video_filename.toLocal8Bit();
        const char *name = bytename.data();
        capture = cvCaptureFromFile(name);
        capture2 = cvCaptureFromFile(name);
        nFps = cvGetCaptureProperty (capture, CV_CAP_PROP_FPS);
        frame = cvQueryFrame(capture);
        firstFrame = frame;
        source = Mat2QImage(firstFrame);
        //source = QImage(filename);

        width = (float)ui->label->width()/source.size().width();
        height = (float)ui->label->height()/source.size().height();

        source = source.scaled(ui->label->width(), ui->label->height());

        buf = QPixmap::fromImage(source);
        buf2 = QPixmap::fromImage(source);
        ui->label->setPixmap(buf);

        idx = 2;

        MainWindow::on_pushButton_3_clicked();
        autoDrawing();

        b->close();


    }
}

void MainWindow::on_backButton2_clicked()
{
    ui->slideOK->setVisible(true);
    ui->slideNOK->setVisible(true);
    ui->manual->setVisible(false);

    ui->okSlide1->setVisible(false);
    ui->okSlide2->setVisible(false);
    ui->nokSlide1->setVisible(false);

    ui->backButton->setVisible(false);
    ui->frontButton->setVisible(false);
    ui->backButton2->setVisible(false);
    ui->frontButton2->setVisible(false);

    video_filename2 = "";
    ui->frontButton2->setStyleSheet("border-image: url(:/8.jpg);");
}

void MainWindow::on_frontButton2_clicked()  // '슬라이드 없다' 다음 버튼
{
    qDebug("front Button clicked!!");

    ui->label->setVisible(true);
    ui->nokSlide1->setVisible(false);
    ui->backButton2->setVisible(false);
    ui->frontButton2->setVisible(false);
    ui->pushButton_2->setVisible(true);

    QByteArray bytename = video_filename2.toLocal8Bit();
    const char *name = bytename.data();
    capture = cvCaptureFromFile(name);
    capture2 = cvCaptureFromFile(name);
    nFps = cvGetCaptureProperty (capture, CV_CAP_PROP_FPS);
    frame = cvQueryFrame(capture);
    firstFrame = frame;
    source = Mat2QImage(firstFrame);

    width = (float)ui->label->width()/source.size().width();
    height = (float)ui->label->height()/source.size().height();

    source = source.scaled(ui->label->width(), ui->label->height());

    buf = QPixmap::fromImage(source);
    buf2 = QPixmap::fromImage(source);
    ui->label->setPixmap(buf);

    idx = 2;

    isAuto = 1;




}

void MainWindow::on_slideOK_clicked()
{
    ui->slideOK->setVisible(false);
    ui->slideNOK->setVisible(false);
    ui->manual->setVisible(false);

    ui->okSlide1->setVisible(true);
    ui->okSlide2->setVisible(true);
    ui->nokSlide1->setVisible(false);

    ui->backButton->setVisible(true);
    ui->frontButton->setVisible(true);
    ui->backButton2->setVisible(false);
    ui->frontButton2->setVisible(false);
}

void MainWindow::on_nokSlide1_clicked()
{
    video_filename2 = QFileDialog::getOpenFileName(this, tr("Open File"), "", "ALL (*.*)");

    if( video_filename2 != "")
    {
        ui->frontButton2->setStyleSheet("border-image: url(:/7.jpg);");
    }
}

void MainWindow::on_manual_clicked()
{
    qDebug("manual button cliked!");
}

void MainWindow::on_okSlide1_clicked()
{
    slide_filename = QFileDialog::getOpenFileName(this, tr("Open File"), "", "ALL (*.*)");


    if( slide_filename != "" && video_filename != "")
    {
        ui->frontButton->setStyleSheet("border-image: url(:/7.jpg);");
    }
}

void MainWindow::on_okSlide2_clicked()
{
    video_filename = QFileDialog::getOpenFileName(this, tr("Open File"), "", "ALL (*.*)");

    if( slide_filename != "" && video_filename != "")
    if( video_filename != "")
    {
        ui->frontButton->setStyleSheet("border-image: url(:/7.jpg);");
    }
}

IplImage* skipNFrames(CvCapture* capture, int n)
{
    for(int i = 0; i < n; ++i)
        if(cvQueryFrame(capture) == NULL)
            return NULL;
    return cvQueryFrame(capture);
}

void MainWindow::on_pushButton_clicked()    // '시작' 누르면 실행되는 함수
{
    qDebug("what");

    a = new Dialog();

    a->show();

    QApplication::processEvents();

    setWaitScreen();    // 합성하는 함수 demo니까 주석

    a->close();
}

void MainWindow::on_pushButton_2_clicked()
{
    a = new Dialog();

    a->show();

    QApplication::processEvents();

    Point2f inputQuad[4];
    Point2f outputQuad[4];
    Mat lambda( 2, 4, CV_32FC1 );
    Mat input, output, myTemp, myTemp2;
    QString tempName;
    QString temp;

    CvSize imgSize;
    imgSize.width = frame->width;
    imgSize.height = frame->height;

    CvVideoWriter *writer2 = cvCreateVideoWriter("outputVideo2.avi", CV_FOURCC ('D','I','V','X'), nFps, imgSize);

    IplImage *largerimage;

    int tempCount[15] = {0};
    int indexCount = 0;

    IplImage *imageArray[30] = {0}; // slide 부분 fame 받기
    IplImage *compareArray[30] = {0};   // 전체 frame 받기
    int imageArrayIdx = 0;

    int pptTime[30] = {0};
    int pptNum[30] = {1};

    int max = 2;    // ppt 최대번호
    int next = 1;

    char indexArray[50000] = {0};   // 삭제하시오.
    /* end */

    imgSize.width = frame->width;
    imgSize.height = frame->height;

    input = firstFrame;
    lambda = Mat::zeros( input.rows, input.cols, input.type() );
    inputQuad[0] = Point2f((float)x1/width, (float)y1/height);
    inputQuad[1] = Point2f((float)x2/width, (float)y2/height);
    inputQuad[2] = Point2f((float)x3/width, (float)y3/height);
    inputQuad[3] = Point2f((float)x4/width, (float)y4/height);

    outputQuad[0] = Point2f(0,0 );
    outputQuad[1] = Point2f(input.cols-1,0);
    outputQuad[2] = Point2f(input.cols-1,input.rows-1);
    outputQuad[3] = Point2f(0, input.rows-1  );

    lambda = getPerspectiveTransform( inputQuad, outputQuad );
    warpPerspective(input,output,lambda,output.size());
    // firstframe rectificate!
    largerimage = cvCloneImage(&IplImage(output));

    CvSize frame_size;
    frame_size.height =
            largerimage->height;
    frame_size.width =
            largerimage->width;

    int myCount = 0;
    int number_of_features = MAX_SIZE;

    do
    {
        //frame = skipNFrames(capture, 24); // 30s sampling
        frame = cvQueryFrame(capture); // all

        if(frame == NULL)
            break;

        input = frame;

        lambda = Mat::zeros( input.rows, input.cols, input.type() );
        inputQuad[0] = Point2f((float)x1/width, (float)y1/height);
        inputQuad[1] = Point2f((float)x2/width, (float)y2/height);
        inputQuad[2] = Point2f((float)x3/width, (float)y3/height);
        inputQuad[3] = Point2f((float)x4/width, (float)y4/height);

        outputQuad[0] = Point2f(0,0 );
        outputQuad[1] = Point2f(input.cols-1,0);
        outputQuad[2] = Point2f(input.cols-1,input.rows-1);
        outputQuad[3] = Point2f(0, input.rows-1  );

        lambda = getPerspectiveTransform( inputQuad, outputQuad );
        warpPerspective(input,output,lambda,output.size());

        IplImage *frame1 = NULL,
                *frame1_1C = NULL,
                *frame2_1C = NULL,
                *eig_image = NULL,
                *temp_image = NULL,
                *pyramid1 = NULL,
                *pyramid2 = NULL;

        allocateOnDemand( &frame1_1C, frame_size, IPL_DEPTH_8U, 1 );
        cvConvertImage(largerimage, frame1_1C, 0);

        allocateOnDemand( &frame1, frame_size, IPL_DEPTH_8U, 3 );
        cvConvertImage(largerimage, frame1, 0);

        cvReleaseImage(&largerimage);
        largerimage = cvCloneImage(&IplImage(output));

        allocateOnDemand(&frame2_1C, frame_size, IPL_DEPTH_8U, 1);
        cvConvertImage(largerimage, frame2_1C, 0);

        CvPoint2D32f frame1_features[MAX_SIZE];

        cvGoodFeaturesToTrack(frame1_1C, eig_image, temp_image,
                              frame1_features, &number_of_features,
                              .01, .01, NULL);

        CvPoint2D32f frame2_features[MAX_SIZE];

        char optical_flow_found_feature[MAX_SIZE];
        float optical_flow_feature_error[MAX_SIZE];

        CvSize optical_flow_window = cvSize(8,8);
        CvTermCriteria optical_flow_termination_criteria
                = cvTermCriteria( CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 20, .3 );

        allocateOnDemand( &pyramid1, frame_size, IPL_DEPTH_8U, 1 );
        allocateOnDemand( &pyramid2, frame_size, IPL_DEPTH_8U, 1 );

        cvCalcOpticalFlowPyrLK(frame1_1C, frame2_1C, pyramid1, pyramid2,
                               frame1_features, frame2_features,
                               number_of_features, optical_flow_window, 5,
                               optical_flow_found_feature,
                               optical_flow_feature_error,
                               optical_flow_termination_criteria, 0 );

        if( time == 3 )
        {
            imageArray[imageArrayIdx] = cvCloneImage(frame1);
            compareArray[imageArrayIdx++] = cvCloneImage(frame);
        }

        for(int i = 0; i < number_of_features; i++)
        {
            if ( optical_flow_found_feature[i] == 0 ) continue;

            CvScalar line_color;
            CvPoint p,q;

            line_color = CV_RGB(255,0,0);

            p.x = (int) frame1_features[i].x;
            p.y = (int) frame1_features[i].y;
            q.x = (int) frame2_features[i].x;
            q.y = (int) frame2_features[i].y;

            double angle = atan2( (double) p.y - q.y, (double) p.x - q.x );
            double hypotenuse = sqrt( square(p.y - q.y) + square(p.x - q.x) );

            q.x = (int) (p.x - 3 * hypotenuse * cos(angle));
            q.y = (int) (p.y - 3 * hypotenuse * sin(angle));

            if( hypotenuse >= 3) myCount++;

        }

        if( time <=14 )
        {
            tempCount[time] = myCount;
        }

        else
        {
            tempCount[time%15] = myCount;

            if( myCount == 0 && tempCount[(time+1)%15] > 0)
            {
                for(int i=0; i<15; i++)
                {
                    if( tempCount[i] > 0 ) indexCount++;
                }

                if( indexCount == 1 )
                {
                    String hash1 = pHashValue(cvarrToMat(imageArray[imageArrayIdx-1]));
                    String hash2 = pHashValue(cvarrToMat(frame1));

                    int check = 0;  // 중복 슬라이드인지 검사
                    int distance = -1;

                    distance = HammingDistance(hash1,hash2);
                    qDebug("%d] this : %d", time, distance);

                    if( distance > 17500 )  // 다른 이미지니까 저장한다 + 앞에 봤던 페이지인지 검사한다.
                    {
                        imageArray[imageArrayIdx] = cvCloneImage(frame1);
                        compareArray[imageArrayIdx] = cvCloneImage(frame);

                        for(int i=0; i<imageArrayIdx; i++)
                        {
                            hash1 = pHashValue(cvarrToMat(imageArray[i]));
                            hash2 = pHashValue(cvarrToMat(imageArray[imageArrayIdx]));

                            distance = HammingDistance(hash1,hash2);
                            qDebug("  %d] this : %d", time, distance);

                            if( distance < 17500 )  // 이미지가 같은 것이다.
                            {
                                qDebug("  %d] this : %d", time, distance);
                                check = 1;
                                pptNum[next] = i+1;
                            }
                        }

                        if( check == 1 )
                        {
                            pptTime[next++] = time-23;
                            imageArrayIdx++;
                        }
                        else
                        {
                            qDebug("------------- next page %d", (time)/24);
                            pptNum[next] = max++;
                            pptTime[next++] = time-23;
                            imageArrayIdx++;
                        }
                    }
                }
                indexCount = 0; // 초기화
            }
        }

        indexArray[time] = myCount;

        myCount = 0;

        cvReleaseImage(&frame1);
        cvReleaseImage(&frame1_1C);
        cvReleaseImage(&frame2_1C);
        cvReleaseImage(&eig_image);
        cvReleaseImage(&temp_image);
        cvReleaseImage(&pyramid1);
        cvReleaseImage(&pyramid2);
        time++;

    }while(frame != NULL);

    qDebug("complete : %d", time);

    pptTime[0] = 1;

    IplImage *gg = cvCreateImage( cvSize(819, 630), IPL_DEPTH_8U, 3);

    for(int i=0; i<imageArrayIdx; i++)
    {
        tempName = "gogo";
        temp = temp.setNum(i+1); // 1로 바꿔
        tempName = tempName + temp + ".jpg";

        fn2 = tempName;
        QByteArray bytename1 = fn2.toLocal8Bit();
        const char *name1 = bytename1.data();

        cvResize(imageArray[i], gg, CV_INTER_LINEAR);

        cvSaveImage(name1, gg);
    }

    QProcess process;
    process.start("img2pdf",QStringList()<< "-noimagerotate" << "-landscape" <<"gogo1.jpg"
                  << "gogo2.jpg" << "gogo3.jpg" << "gogo4.jpg" <<"output.pdf");

    process.waitForFinished();
    process.destroyed();

    a->close();
}

void MainWindow::setWaitScreen()
{
    Point2f inputQuad[4];
    Point2f outputQuad[4];
    Mat lambda( 2, 4, CV_32FC1 );
    Mat input, output, myTemp, myTemp2;
    QString tempName;
    QString temp;

    CvSize imgSize;
    imgSize.width = frame->width;
    imgSize.height = frame->height;

    CvVideoWriter *writer2 = cvCreateVideoWriter("Output.avi", CV_FOURCC ('D','I','V','X'), nFps, imgSize);

    IplImage *largerimage;

    int tempCount[15] = {0};
    int indexCount = 0;

    IplImage *imageArray[30] = {0}; // slide 부분 fame 받기
    IplImage *compareArray[30] = {0};   // 전체 frame 받기
    int imageArrayIdx = 0;

    int pptTime[30] = {0};
    int pptNum[30] = {1};

    int max = 2;    // ppt 최대번호
    int next = 1;

    char indexArray[50000] = {0};   // 삭제하시오.
    /* end */

    imgSize.width = frame->width;
    imgSize.height = frame->height;

    input = firstFrame;
    lambda = Mat::zeros( input.rows, input.cols, input.type() );
    inputQuad[0] = Point2f((float)x1/width, (float)y1/height);
    inputQuad[1] = Point2f((float)x2/width, (float)y2/height);
    inputQuad[2] = Point2f((float)x3/width, (float)y3/height);
    inputQuad[3] = Point2f((float)x4/width, (float)y4/height);

    outputQuad[0] = Point2f(0,0 );
    outputQuad[1] = Point2f(input.cols-1,0);
    outputQuad[2] = Point2f(input.cols-1,input.rows-1);
    outputQuad[3] = Point2f(0, input.rows-1  );

    lambda = getPerspectiveTransform( inputQuad, outputQuad );
    warpPerspective(input,output,lambda,output.size());
    // firstframe rectificate!
    largerimage = cvCloneImage(&IplImage(output));

    CvSize frame_size;
    frame_size.height =
            largerimage->height;
    frame_size.width =
            largerimage->width;

    int myCount = 0;
    int number_of_features = MAX_SIZE;

    do
    {
        //frame = skipNFrames(capture, 24); // 30s sampling
        frame = cvQueryFrame(capture); // all

        if(frame == NULL)
            break;

        input = frame;

        lambda = Mat::zeros( input.rows, input.cols, input.type() );
        inputQuad[0] = Point2f((float)x1/width, (float)y1/height);
        inputQuad[1] = Point2f((float)x2/width, (float)y2/height);
        inputQuad[2] = Point2f((float)x3/width, (float)y3/height);
        inputQuad[3] = Point2f((float)x4/width, (float)y4/height);

        outputQuad[0] = Point2f(0,0 );
        outputQuad[1] = Point2f(input.cols-1,0);
        outputQuad[2] = Point2f(input.cols-1,input.rows-1);
        outputQuad[3] = Point2f(0, input.rows-1  );

        lambda = getPerspectiveTransform( inputQuad, outputQuad );
        warpPerspective(input,output,lambda,output.size());

        IplImage *frame1 = NULL,
                *frame1_1C = NULL,
                *frame2_1C = NULL,
                *eig_image = NULL,
                *temp_image = NULL,
                *pyramid1 = NULL,
                *pyramid2 = NULL;

        allocateOnDemand( &frame1_1C, frame_size, IPL_DEPTH_8U, 1 );
        cvConvertImage(largerimage, frame1_1C, 0);

        allocateOnDemand( &frame1, frame_size, IPL_DEPTH_8U, 3 );
        cvConvertImage(largerimage, frame1, 0);

        cvReleaseImage(&largerimage);
        largerimage = cvCloneImage(&IplImage(output));

        allocateOnDemand(&frame2_1C, frame_size, IPL_DEPTH_8U, 1);
        cvConvertImage(largerimage, frame2_1C, 0);

        CvPoint2D32f frame1_features[MAX_SIZE];

        cvGoodFeaturesToTrack(frame1_1C, eig_image, temp_image,
                              frame1_features, &number_of_features,
                              .01, .01, NULL);

        CvPoint2D32f frame2_features[MAX_SIZE];

        char optical_flow_found_feature[MAX_SIZE];
        float optical_flow_feature_error[MAX_SIZE];

        CvSize optical_flow_window = cvSize(8,8);
        CvTermCriteria optical_flow_termination_criteria
                = cvTermCriteria( CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 20, .3 );

        allocateOnDemand( &pyramid1, frame_size, IPL_DEPTH_8U, 1 );
        allocateOnDemand( &pyramid2, frame_size, IPL_DEPTH_8U, 1 );

        cvCalcOpticalFlowPyrLK(frame1_1C, frame2_1C, pyramid1, pyramid2,
                               frame1_features, frame2_features,
                               number_of_features, optical_flow_window, 5,
                               optical_flow_found_feature,
                               optical_flow_feature_error,
                               optical_flow_termination_criteria, 0 );

        if( time == 3 )
        {
            imageArray[imageArrayIdx] = cvCloneImage(frame1);
            compareArray[imageArrayIdx++] = cvCloneImage(frame);
        }

        for(int i = 0; i < number_of_features; i++)
        {
            if ( optical_flow_found_feature[i] == 0 ) continue;

            CvScalar line_color;
            CvPoint p,q;

            line_color = CV_RGB(255,0,0);

            p.x = (int) frame1_features[i].x;
            p.y = (int) frame1_features[i].y;
            q.x = (int) frame2_features[i].x;
            q.y = (int) frame2_features[i].y;

            double angle = atan2( (double) p.y - q.y, (double) p.x - q.x );
            double hypotenuse = sqrt( square(p.y - q.y) + square(p.x - q.x) );

            q.x = (int) (p.x - 3 * hypotenuse * cos(angle));
            q.y = (int) (p.y - 3 * hypotenuse * sin(angle));

            if( hypotenuse >= 3) myCount++;

        }

        if( time <=14 )
        {
            tempCount[time] = myCount;
        }

        else
        {
            tempCount[time%15] = myCount;

            if( myCount == 0 && tempCount[(time+1)%15] > 0)
            {
                for(int i=0; i<15; i++)
                {
                    if( tempCount[i] > 0 ) indexCount++;
                }

                if( indexCount == 1 )
                {
                    String hash1 = pHashValue(cvarrToMat(imageArray[imageArrayIdx-1]));
                    String hash2 = pHashValue(cvarrToMat(frame1));

                    int check = 0;  // 중복 슬라이드인지 검사
                    int distance = -1;

                    distance = HammingDistance(hash1,hash2);
                    qDebug("%d] this : %d", time, distance);

                    if( distance > 17500 )  // 다른 이미지니까 저장한다 + 앞에 봤던 페이지인지 검사한다.
                    {
                        imageArray[imageArrayIdx] = cvCloneImage(frame1);
                        compareArray[imageArrayIdx] = cvCloneImage(frame);

                        for(int i=0; i<imageArrayIdx; i++)
                        {
                            hash1 = pHashValue(cvarrToMat(imageArray[i]));
                            hash2 = pHashValue(cvarrToMat(imageArray[imageArrayIdx]));

                            distance = HammingDistance(hash1,hash2);
                            qDebug("  %d] this : %d", time, distance);

                            if( distance < 17500 )  // 이미지가 같은 것이다.
                            {
                                qDebug("  %d] this : %d", time, distance);
                                check = 1;
                                pptNum[next] = i+1;
                            }
                        }

                        if( check == 1 )
                        {
                            pptTime[next++] = time-23;
                            imageArrayIdx++;
                        }
                        else
                        {
                            qDebug("------------- next page %d", (time)/24);
                            pptNum[next] = max++;
                            pptTime[next++] = time-23;
                            imageArrayIdx++;
                        }
                    }
                }
                indexCount = 0; // 초기화
            }
        }

        indexArray[time] = myCount;

        myCount = 0;

        cvReleaseImage(&frame1);
        cvReleaseImage(&frame1_1C);
        cvReleaseImage(&frame2_1C);
        cvReleaseImage(&eig_image);
        cvReleaseImage(&temp_image);
        cvReleaseImage(&pyramid1);
        cvReleaseImage(&pyramid2);
        time++;



    }while(frame != NULL);

    qDebug("complete : %d", time);

    pptTime[0] = 1;

    printf("★★★★★★★★\n");
    for(int i=1; i<30; i++)
    {
        qDebug("%dp -> %dp : %ds", i, i+1, pptTime[i]);
    }

    /* 추출된 시간으로 합성하기 */

    IplImage *largerimage2;
    IplImage *img2 = cvCreateImage( cvSize(rightX-leftX, downY-upY), IPL_DEPTH_8U, 3); // 최대, 최소값 구하기
    IplImage *bigImg;

    qDebug("original %d, %d", rightX-leftX, downY-upY);

    IplImage *xframe;

    time = 1;
    next = 0;

    do
    {
        frame = cvQueryFrame(capture2); // all

        if(frame == NULL) break;

        IplImage *cframe = cvCloneImage(frame);

        if( time == pptTime[next])
        {
            xframe = cvCloneImage(compareArray[next]);

            tempName = "C:\\Users\\kjy\\Desktop\\slide\\slide";
            temp = temp.setNum(pptNum[next++]); // 1로 바꿔
            tempName = tempName + temp + ".jpg";

            fn2 = tempName;
            QByteArray bytename1 = fn2.toLocal8Bit();
            const char *name1 = bytename1.data();

            largerimage2 = cvLoadImage(name1);
            cvResize(largerimage2, img2, CV_INTER_LINEAR);

            /* 이 아래로 역rect한 이미지를 씌우는 작업이라능 */

            myTemp = largerimage2;

            lambda = Mat::zeros( myTemp.rows, myTemp.cols, myTemp.type() );
            outputQuad[0] = Point2f(0, 0);
            outputQuad[1] = Point2f(myTemp.cols-1, 0);
            outputQuad[2] = Point2f(myTemp.cols-1, myTemp.rows-1);
            outputQuad[3] = Point2f(0, myTemp.rows-1);

            inputQuad[0] = Point2f((float)x1, (float)y1);
            inputQuad[1] = Point2f((float)x2, (float)y2);
            inputQuad[2] = Point2f((float)x3, (float)y3);
            inputQuad[3] = Point2f((float)x4, (float)y4);

            lambda = getPerspectiveTransform( outputQuad, inputQuad );
            warpPerspective(myTemp,myTemp2,lambda,myTemp2.size() );

            largerimage = cvCloneImage(&IplImage(myTemp2));
        }

        if( indexArray[time-1] == 0 ) drawImage(frame, largerimage, x1, y1, xframe, cframe);    // 나머지 경우 weak 검사
        else if( indexArray[time-1] > 5 )   // optical flow 값이 크니까 strong 검사
        {
            drawImage(frame, largerimage, 0, y1, xframe, cframe);
        }
        else drawImage(frame, largerimage, x1, y1, xframe, cframe);

        cvWriteFrame(writer2,frame);
        time++;

        cvReleaseImage(&cframe);

    }while(frame != NULL);

    cvReleaseVideoWriter(&writer2);
    cvReleaseImage(&frame);
    cvReleaseImage(&xframe);
    qDebug("complete : %d", time);
}

class AutoSearch
{
public:
    Mat autoSize;
    Point2f point[4];
};


void MainWindow::on_pushButton_3_clicked()
{
    QString tempName;
    QString temp;

    cv::Mat ori;
    int count = 0;

    AutoSearch as[3];

    qDebug("%s", video_filename);

    QByteArray bytename = video_filename.toLocal8Bit();
    const char *name = bytename.data();
    CvCapture* capture3 = cvCaptureFromFile(name);

    if( slide_filename.length() == 37 ) tempName = "C:\\Users\\kjy\\Desktop\\first.jpg";
    else tempName = "C:\\Users\\kjy\\Desktop\\first2.jpg";

    fn2 = tempName;
    QByteArray bytename1 = fn2.toLocal8Bit();
    const char *name1 = bytename1.data();

    cv::Mat db_original = cvLoadImageM(name1);
    cv::Mat db;
    cv::resize( db_original, db, cv::Size(db_original.cols/2,
                                          db_original.rows/2),0,0,CV_INTER_NN);

    // SIFT feature detector and feature extractor
    cv::SiftFeatureDetector detector( /*0.05, 5.0*/ 500 );
    cv::SiftFeatureDetector detector1( /*0.05, 5.0*/ 2000 );
    cv::SiftDescriptorExtractor extractor( 3.0 );

    std::vector<cv::KeyPoint> kps_db;

    detector.detect( db, kps_db );

    cv::Mat dscr_db;
    extractor.compute( db, kps_db, dscr_db);

    IplImage *gg123;

    while (true) {
        bool frame_valid = true;

        cv::Mat frame_original;
        cv::Mat frame;

        try {
            gg123 = cvQueryFrame(capture3);

            frame_original = cvarrToMat(gg123);

            cv::resize(frame_original,frame,cv::Size(frame_original.cols/2,
                                                     frame_original.rows/2),0,0,CV_INTER_NN); // downsample 1/2x
            ori = frame_original.clone();


            count++;
            qDebug("%d", count);

            if(count ==4 )
                break;

        } catch(cv::Exception& e) {

            std::cerr << "Exception occurred. Ignoring frame... " << e.err
                      << std::endl;
            frame_valid = false;
        }

        if (frame_valid) {
            try {

                cv::Mat grayframe;
                cv::cvtColor(frame, grayframe, CV_BGR2GRAY);
                cv::equalizeHist(grayframe,grayframe);

                std::vector<cv::KeyPoint> kps_frame;
                detector1.detect( grayframe, kps_frame);

                cv::Mat dscr_frame;
                extractor.compute( grayframe, kps_frame, dscr_frame);

                cv::FlannBasedMatcher matcher;
                std::vector<cv::DMatch> matches;
                matcher.match(dscr_db, dscr_frame, matches);

                double max_dist = 0.0, min_dist = 100.0;

                for(int i=0; i<matches.size(); i++) {

                    double dist = matches[i].distance;
                    if ( dist < min_dist ) min_dist = dist;
                    if ( dist > max_dist ) max_dist = dist;

                }
                std::vector<cv::DMatch> good_matches;

                float aa = 0.0;
                float bb = 0.0;
                int cnt = 0;

                if( slide_filename.length() == 37 )
                {
                    for (int i=0; i<matches.size(); i++)
                    {
                        if (matches[i].distance <= 3.0*min_dist)
                        {
                            aa = ((((int)kps_db[ matches[i].queryIdx ].pt.x - ((int)kps_frame[ matches[i].trainIdx ].pt.x + 320))) +0.0) /
                                    ((int)kps_db[ matches[i].queryIdx ].pt.y - (int)kps_frame[ matches[i].trainIdx ].pt.y);

                            bb = (((int)kps_db[ matches[i].queryIdx ].pt.x - ((int)kps_frame[ matches[i].trainIdx ].pt.x + 320)) *
                                    ((int)kps_db[ matches[i].queryIdx ].pt.x - ((int)kps_frame[ matches[i].trainIdx ].pt.x + 320))) +
                                    (((int)kps_db[ matches[i].queryIdx ].pt.y - (int)kps_frame[ matches[i].trainIdx ].pt.y) *
                                    ((int)kps_db[ matches[i].queryIdx ].pt.y - (int)kps_frame[ matches[i].trainIdx ].pt.y));

                            if ( (aa > 2.7 && aa < 3.5) && (bb > 105000 && bb < 135000))
                            {
                                good_matches.push_back( matches[i] );
                                //                            //cout << bb << endl;
                                cnt++;
                            }
                        }
                    }
                }

                else
                {
                    for (int i=0; i<matches.size(); i++)
                    {

                        good_matches.push_back( matches[i] );
                        //                            //cout << bb << endl;
                        cnt++;

                    }
                }

                cv::Mat img_matches;
                cv::drawMatches(db, kps_db, frame, kps_frame, good_matches,
                                img_matches, cv::Scalar::all(-1), cv::Scalar::all(-1),
                                std::vector<char>(),
                                cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

                ////////////////////////////////////////////////
                ////////////////////////////////////////////////

                std::vector<Point2f> obj;
                std::vector<Point2f> scene;

                for( int i = 0; i < good_matches.size(); i++ )
                {

                    obj.push_back( kps_db[ good_matches[i].queryIdx ].pt );
                    scene.push_back( kps_frame[ good_matches[i].trainIdx ].pt );
                }

                if( cnt > 4){
                    Mat H = findHomography( obj, scene, CV_RANSAC );

                    std::vector<Point2f> obj_corners(4);
                    obj_corners[0] = cvPoint(0,0); obj_corners[1] = cvPoint( db.cols, 0 );
                    obj_corners[2] = cvPoint( db.cols, db.rows ); obj_corners[3] = cvPoint( 0, db.rows );
                    std::vector<Point2f> scene_corners(4);

                    perspectiveTransform( obj_corners, scene_corners, H);

                    scene_corners[0].x *= 2; scene_corners[0].y *= 2;
                    scene_corners[1].x *= 2; scene_corners[1].y *= 2;
                    scene_corners[2].x *= 2; scene_corners[2].y *= 2;
                    scene_corners[3].x *= 2; scene_corners[3].y *= 2;

                    line( ori, scene_corners[0] /*+ Point2f( db.cols, 0)*/, scene_corners[1] /*+ Point2f( db.cols, 0)*/, Scalar(0, 255, 0), 4 );
                    line( ori, scene_corners[1] /*+ Point2f( db.cols, 0)*/, scene_corners[2] /*+ Point2f( db.cols, 0)*/, Scalar( 0, 255, 0), 4 );
                    line( ori, scene_corners[2] /*+ Point2f( db.cols, 0)*/, scene_corners[3] /*+ Point2f( db.cols, 0)*/, Scalar( 0, 255, 0), 4 );
                    line( ori, scene_corners[3] /*+ Point2f( db.cols, 0)*/, scene_corners[0] /*+ Point2f( db.cols, 0)*/, Scalar( 0, 255, 0), 4 );

                    as[count-1].point[0] = scene_corners[0];
                    as[count-1].point[1] = scene_corners[1];
                    as[count-1].point[2] = scene_corners[2];
                    as[count-1].point[3] = scene_corners[3];
                }

                else
                {
                    db_original = cv::imread("C:\\133.jpg",CV_LOAD_IMAGE_GRAYSCALE);

                    cv::resize( db_original, db, cv::Size(db_original.cols/2,
                                                          db_original.rows/2),0,0,CV_INTER_NN);
                    detector.detect( db, kps_db );

                    extractor.compute( db, kps_db, dscr_db);
                }

                if(count <4)
                    as[count-1].autoSize = ori.clone();

            } catch(cv::Exception& e) {
                std::cerr << "Exception occurred. Ignoring frame... " << e.err
                          << std::endl;
            }
        }

        if (cv::waitKey(30) >= 0) break;
    }

    //imshow("1", as[0].autoSize);
    //imshow("2", as[1].autoSize);
    //imshow("3", as[2].autoSize);

    qDebug("%d %d", (int)as[0].point[0].x, (int)as[0].point[0].y);
    qDebug("%d %d", as[0].point[1].x, as[0].point[1].y);
    qDebug("%d %d", as[0].point[2].x, as[0].point[2].y);
    qDebug("%d %d", as[0].point[3].x, as[0].point[3].y);

    autoX1[0] = (int)as[0].point[0].x; autoY1[0] = (int)as[0].point[0].y;
    autoX2[0] = (int)as[0].point[1].x; autoY2[0] = (int)as[0].point[1].y;
    autoX3[0] = (int)as[0].point[2].x; autoY3[0] = (int)as[0].point[2].y;
    autoX4[0] = (int)as[0].point[3].x; autoY4[0] = (int)as[0].point[3].y;

    autoX1[1] = (int)as[1].point[0].x; autoY1[1] = (int)as[1].point[0].y;
    autoX2[1] = (int)as[1].point[1].x; autoY2[1] = (int)as[1].point[1].y;
    autoX3[1] = (int)as[1].point[2].x; autoY3[1] = (int)as[1].point[2].y;
    autoX4[1] = (int)as[1].point[3].x; autoY4[1] = (int)as[1].point[3].y;

    autoX1[2] = (int)as[2].point[0].x; autoY1[2] = (int)as[2].point[0].y;
    autoX2[2] = (int)as[2].point[1].x; autoY2[2] = (int)as[2].point[1].y;
    autoX3[2] = (int)as[2].point[2].x; autoY3[2] = (int)as[2].point[2].y;
    autoX4[2] = (int)as[2].point[3].x; autoY4[2] = (int)as[2].point[3].y;
    waitKey(0);

}

void MainWindow::on_backButton2_2_clicked()
{

}

void MainWindow::on_pushButton_4_clicked()
{
    qDebug("수동 버튼 클릭");
    isAuto = 1;

    buf2 = QPixmap::fromImage(source);
    ui->label->setPixmap(buf2);
}

void MainWindow::autoDrawing()
{
    buf2 = QPixmap::fromImage(source);

    qDebug("main clicked!");

    QString temp;
    QString temp2;

    //temp = temp.setNum(event->pos().x());
    //temp2 = temp2.setNum(event->pos().y());

    QPainter p(&buf2);
    p.setPen(Qt::red);

    QPen linepen(Qt::red);
    linepen.setCapStyle(Qt::RoundCap);
    linepen.setWidth(5);

    p.setPen(linepen);

    x1 = autoX1[autoIdx]; y1 = autoY1[autoIdx];
    x2 = autoX2[autoIdx]; y2 = autoY2[autoIdx];
    x3 = autoX3[autoIdx]; y3 = autoY3[autoIdx];
    x4 = autoX4[autoIdx]; y4 = autoY4[autoIdx++];

    p.drawLine(x1, y1, x2, y2);
    p.drawLine(x2, y2, x3, y3);
    p.drawLine(x3, y3, x4, y4);
    p.drawLine(x1, y1, x4, y4);

    ui->label->setPixmap(buf2);

    //    buf2 = QPixmap::fromImage(source);

    //    QPolygon region;
    //    region << QPoint(x1-10, y1-20) << QPoint(x2-10, y2-20) << QPoint(x3-10, y3-20) << QPoint(x4-10, y4-20);

    //    QPainterPath path;
    //    path.addPolygon(region);

    //    cutout = QImage(source.size(), QImage::Format_ARGB32_Premultiplied);
    //    //cutout.fill(Qt::transparent);

    //    cutout.fill(QColor(Qt::black).rgb());

    //    QPainter p1(&cutout);
    //    p1.setClipPath(path);
    //    p1.drawImage(0, 0, source);
    //    p1.end();

    //cutout.save("test.jpg");
    //    x1 = (x1-10);
    //    x2 = (x2-10);
    //    x3 = (x3-10);
    //    x4 = (x4-10);
    //    y1 = (y1-20);
    //    y2 = (y2-20);
    //    y3 = (y3-20);
    //    y4 = (y4-20);

    p.end();

    setMax();

}

void MainWindow::on_pushButton_5_clicked()  // 새로 고침
{
    isAuto = 0;
    autoDrawing();
    if( autoIdx == 3 ) autoIdx = 0;
}

void MainWindow::on_pushButton_6_clicked()
{



    Mat img, imgLaplacian, imgResult;

    //------------------------------------------------------------------------------------------- test, first of all
    // now do it by hand
    img = (Mat_<uchar>(4,4) << 0,1,2,3,4,5,6,7,8,9,0,11,12,13,14,15);

    // first, the good result
    Laplacian(img, imgLaplacian, CV_8UC1);
    cout << "let opencv do it" << endl;
    cout << imgLaplacian << endl;

    Mat kernel = (Mat_<float>(3,3) <<
                  0,  1, 0,
                  1, -4, 1,
                  0,  1, 0);
    int window_size = 3;

    // now, reaaallly by hand
    // note that, for avoiding padding, the result image will be smaller than the original one.
    Mat frame, frame32;
    Rect roi;
    imgLaplacian = Mat::zeros(img.size(), CV_32F);
    for(int y=0; y<img.rows-window_size/2-1; y++) {
        for(int x=0; x<img.cols-window_size/2-1; x++) {
            roi = Rect(x,y, window_size, window_size);
            frame = img(roi);
            frame.convertTo(frame, CV_32F);
            frame = frame.mul(kernel);
            float v = sum(frame)[0];
            imgLaplacian.at<float>(y,x) = v;
        }
    }
    imgLaplacian.convertTo(imgLaplacian, CV_8U);
    cout << "dudee" << imgLaplacian << endl;

    // a little bit less "by hand"..
    // using cv::filter2D
    filter2D(img, imgLaplacian, -1, kernel);
    cout << imgLaplacian << endl;


    //------------------------------------------------------------------------------------------- real stuffs now
    QString tempName = "C:\\Qt\\Qt5.2.1\\Tools\\QtCreator\\bin\\build-untitled-Desktop_Qt_5_2_1_MSVC2010_32bit-Release\\gogo1.jpg";

    //        tempName.append("1");
    //        tempName.append(".jpg");

    fn2 = tempName;
    QByteArray bytename1 = fn2.toLocal8Bit();
    const char *name1 = bytename1.data();

    img = cvLoadImageM(name1);
    // ok, now try different kernel
    kernel = (Mat_<float>(3,3) <<
              1,  1, 1,
              1, -8, 1,
              1,  1, 1); // another approximation of second derivate, more stronger

    // do the laplacian filtering as it is
    // well, we need to convert everything in something more deeper then CV_8U
    // because the kernel has some negative values,
    // and we can expect in general to have a Laplacian image with negative values
    // BUT a 8bits unsigned int (the one we are working with) can contain values from 0 to 255
    // so the possible negative number will be truncated
    filter2D(img, imgLaplacian, CV_32F, kernel);
    img.convertTo(img, CV_32F);
    imgResult = img - imgLaplacian;

    // convert back to 8bits gray scale
    imgResult.convertTo(imgResult, CV_8U);
    imgLaplacian.convertTo(imgLaplacian, CV_8U);

    namedWindow("laplacian", CV_WINDOW_AUTOSIZE);
    imshow( "laplacian", imgLaplacian );

    namedWindow("result", CV_WINDOW_AUTOSIZE);
    imshow( "result", imgResult );

    imwrite("C:\\Qt\\Qt5.2.1\\Tools\\QtCreator\\bin\\build-untitled-Desktop_Qt_5_2_1_MSVC2010_32bit-Release\\test123.jpg",imgResult);


    QProcess process;
    process.start("img2pdf",QStringList()<< "-noimagerotate" << "-landscape" <<"test123.jpg"
                  << "test223.jpg" << "test323.jpg" << "test423.jpg" <<"output.pdf");

    //        process.start("img2pdf",QStringList()<< "-noimagerotate" << "-landscape" <<"gogo1.jpg"
    //                      << "gogo2.jpg" << "gogo3.jpg" << "gogo4.jpg" <<"output.pdf");


    process.waitForFinished();
    process.destroyed();
}
