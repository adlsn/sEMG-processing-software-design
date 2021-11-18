#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <Qtcore/QFile>
#include <QtWidgets/QMessageBox>
#include <QtGui/QDesktopServices>
#include "workthread.h"
#include <QtCharts>


#include "QLineSeries"
#include "QValueAxis"
#include "QTimer"
#include "QTime"
#include "QList"
#include "qmath.h"
#include "QPointF"
#include "QDebug"
#include "cglobal.h"

using namespace QtCharts;

int datashow=0;//当为0的时候，正常。

int num=0;
int num2=1;

QString old_str;
QString new_str;

int left_emgroute=1;
int right_emgroute=5;
//左腿
QChart *left_chart;
QLineSeries *left_series;

//右腿
QChart *right_chart;
QLineSeries *right_series;

//QList<double> dataList;//存储业务数据
int maxSize = 5000;

//QTimer updateTimer;
int timeId;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    setFixedSize(1854,520);// 界面大小设置
    ui->setupUi(this);

    ui->outputloc->setText("D:\\");
    ui->outputname->setText("output.txt");

    ui->pushButton_Send->setEnabled(false);
    ui->lineEdit_IP->setText("192.168.16.254");//TCP服务器IP地址
    //ui->lineEdit_IP->setText("127.0.0.1");
    ui->lineEdit_Port->setText("8086");//TCP服务器端口地址

    //设置显示图表------------------------------------------------------------------------
    //左腿
    left_chart = new QChart;
    QChartView *left_chartView = new QChartView(left_chart);
    left_chartView->setRubberBand(QChartView::RectangleRubberBand);//橡皮筋类，框选区域

    left_series = new QLineSeries;//折线图
    left_chart->addSeries(left_series);

    for(int i=0;i<maxSize;++i){
        left_series->append(i,0);
    }//初始化

    left_series->setUseOpenGL(true);//openGl 加速
    qDebug()<<left_series->useOpenGL();

    QValueAxis *axisX = new QValueAxis;
    axisX->setRange(0,maxSize);
    axisX->setLabelFormat("%g");
    axisX->setTitleText("axisX");

    QValueAxis *axisY = new QValueAxis;
    axisY->setRange(0,3.3);
    axisY->setTitleText("axisY");

    left_chart->setAxisX(axisX,left_series);
    left_chart->setAxisY(axisY,left_series);
    left_chart->legend()->hide();
    left_chart->setTitle("肌电通道1");
    ui->emg1->setChecked(true);//肌电通道1被选中

    QVBoxLayout *left_layout = ui->verticalLayout;
    left_layout->addWidget(left_chartView);

    //右腿
    right_chart = new QChart;
    QChartView *right_chartView = new QChartView(right_chart);
    right_chartView->setRubberBand(QChartView::RectangleRubberBand);//橡皮筋类，框选区域

    right_series = new QLineSeries;//折线图
    right_chart->addSeries(right_series);

    for(int i=0;i<maxSize;++i){
        right_series->append(i,0);
    }//初始化

    right_series->setUseOpenGL(true);//openGl 加速
    qDebug()<<right_series->useOpenGL();

    QValueAxis *axisX2 = new QValueAxis;
    axisX2->setRange(0,maxSize);
    axisX2->setLabelFormat("%g");
    axisX2->setTitleText("axisX");

    QValueAxis *axisY2 = new QValueAxis;
    axisY2->setRange(0,3.3);
    axisY2->setTitleText("axisY");

    right_chart->setAxisX(axisX2,right_series);
    right_chart->setAxisY(axisY2,right_series);
    right_chart->legend()->hide();
    right_chart->setTitle("肌电通道5");
    ui->emg5->setChecked(true);//肌电通道5被选中

    QVBoxLayout *right_layout = ui->verticalLayout_2;
    right_layout->addWidget(right_chartView);

    timeId = startTimer(30);//开启定时器，30ms
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_Connect_clicked()
{
    if(ui->pushButton_Connect->text() == tr("连 接"))
    {
        //发送按键使能
        ui->pushButton_Send->setEnabled(true);
        //修改按键文字
        ui->pushButton_Connect->setText("断开连接");
        thread1=new WorkThread();//新建线程
        connect(thread1, SIGNAL(wifidataSignal_1(QString)), this, SLOT(Displaywifidata_01(QString)));
        connect(thread1, SIGNAL(wifidataSignal_2(QString)), this, SLOT(Displaywifidata_02(QString)));

        QObject::connect(this, SIGNAL(Send_IP(QString)), thread1, SLOT(socket_IP(QString)));
        QObject::connect(this, SIGNAL(Send_Port(int)), thread1, SLOT(socket_Port(int)));

        thread1->start();

        emit Send_IP(ui->lineEdit_IP->text());
        emit Send_Port(ui->lineEdit_Port->text().toInt());

    }
    else
    {
        //断开连接
        thread1->terminate();
        //修改按键文字
        ui->pushButton_Connect->setText("连 接");
        ui->pushButton_Send->setEnabled(false);
    }
}

void MainWindow::on_pushButton_Send_clicked()
{
    qDebug() << "Send: " << ui->textEdit_Send->toPlainText();
    //获取文本框内容并以ASCII码形式发送
    //    socket->write(ui->textEdit_Send->toPlainText().toLatin1());
    //    socket->flush();
}

void MainWindow::on_pushButton_datasaving2_clicked()
{
    //writetext
    if(ui->outputname->text()==NULL)
    {
        QMessageBox::information(this,"提示","你没有输入“输出文件名称”！");
    }else if(ui->outputloc->text()==NULL)
    {
        QMessageBox::information(this,"提示","你没有选中“输出到文件夹”的地址！");
    }else
    {
        QFile file(ui->outputloc->text()+"/"+ui->outputname->text());
        file.open( QIODevice::Text|QIODevice::ReadWrite | QIODevice::Truncate);
        QTextStream out(&file);
        if(!ui->textEdit_Recv->toPlainText().isEmpty()){
            QString pp=ui->textEdit_Recv->toPlainText();
            out << ui->textEdit_Recv->toPlainText();
        }
        file.close();
    }

    //    QString str=ui->textEdit_Recv_data->toPlainText();
    //    str.replace("\n","\r\n");
    //    qDebug()<<str;
    //    QFile file("D:/2.txt");
    //    if(!file.open(QIODevice::WriteOnly|QIODevice::Append)){
    //        QMessageBox::warning(this,tr("错误"),tr("文件打开失败"));
    //        return;
    //    }
    //    QTextStream in(&file);
    //    in<<str;
    //    QMessageBox::information(this,tr("提示"),tr("数据保存成功,按ok后自动打开"));
    //    QDesktopServices::openUrl(QUrl("D:/2.txt"));
}

void MainWindow::on_pushButton_dataclear_1_clicked()
{
    ui->textEdit_Recv_data->clear();
}

void MainWindow::on_pushButton_dataclear_2_clicked()
{
    ui->textEdit_Recv->clear();
}

void MainWindow::Displaywifidata_01(QString str_01)
{
//   ui->textEdit_Recv_data->append(old_str);//显示原始数据
//   qDebug()<<"size-->";
//   old_str.clear();
}

void MainWindow::Displaywifidata_02(QString str_02)
{
    ui->textEdit_Recv->append(new_str);//显示真实的一帧数据
    //new_str.clear();

    //获得double类型数据
    double left_now_data[4000]={0};
    double right_now_data[4000]={0};
    QString y=",";
    QString z="\r";

    int loc_huanhang_first=0;
    int loc_huanhang_end=0;
    int k=0;

    while((loc_huanhang_end=(new_str.indexOf(z,loc_huanhang_end+6)))>0)
    {
        QString dou_data=new_str.mid(loc_huanhang_first,loc_huanhang_end-loc_huanhang_first);
        loc_huanhang_first=loc_huanhang_end+2;
        int i=0;
        int loc_douhao_first=0;
        int loc_douhao_end=0;
        while(1)//提取指定通道的肌电值,,左腿
        {
            loc_douhao_end=(dou_data.indexOf(y,loc_douhao_end+1));
            i++;
            if(i==left_emgroute)
            {
                   left_now_data[k]=dou_data.mid(loc_douhao_first,loc_douhao_end-loc_douhao_first).toDouble();
                   break;
            }
            loc_douhao_first=loc_douhao_end+1;
        }

//        i=0;
//        loc_douhao_first=0;
//        loc_douhao_end=0;
        loc_douhao_first=loc_douhao_end+1;
        while(1)//提取指定通道的肌电值,,右腿
        {
            loc_douhao_end=(dou_data.indexOf(y,loc_douhao_end+1));
            i++;
            if(i==right_emgroute)
            {
                   right_now_data[k]=dou_data.mid(loc_douhao_first,loc_douhao_end-loc_douhao_first).toDouble();
                   break;
            }
            loc_douhao_first=loc_douhao_end+1;
        }
        k++;
    }
    new_str.clear();

    int size = k;//数据个数

    //qDebug()<<"size-->"<<size;
    if(isVisible()){
        QVector<QPointF> oldPoints = left_series->pointsVector();//Returns the points in the series as a vector
        QVector<QPointF> points;

        for(int i=size;i<oldPoints.count();++i){
            points.append(QPointF(i-size ,oldPoints.at(i).y()));//替换数据用
        }

        qint64 sizePoints = points.count();
        for(int k=0;k<size;++k){
            points.append(QPointF(k+sizePoints,left_now_data[k]));//xx为新数据
        }
        left_series->replace(points);

        QVector<QPointF> old2Points = right_series->pointsVector();//Returns the points in the series as a vector
        QVector<QPointF> points2;

        for(int i=size;i<old2Points.count();++i){
            points2.append(QPointF(i-size ,old2Points.at(i).y()));//替换数据用
        }

        qint64 sizePoints2 = points2.count();
        for(int k=0;k<size;++k){
            points2.append(QPointF(k+sizePoints2,right_now_data[k]));//xx为新数据
        }
        right_series->replace(points2);
    }

}

double MainWindow::getData(double time){

    double s = qCos( time * M_PI * 2 ) ;
    return s;
}

void MainWindow::timerEvent(QTimerEvent *event){
    if(event->timerId()==timeId){//定时器到时间,//模拟数据填充

        //ui->label_5->setText(QString::number(ui->textEdit_Recv_data->document()->lineCount()));
        ui->label_6->setText(QString::number(ui->textEdit_Recv->document()->lineCount()));

//        static QTime dataTime(QTime::currentTime());
//        long int eltime = dataTime.elapsed();
//        static int lastpointtime = 0;

//        int size = (eltime - lastpointtime);//数据个数

//        qDebug()<<"size-->"<<size;
//        if(isVisible()){
//            QVector<QPointF> oldPoints = left_series->pointsVector();//Returns the points in the series as a vector
//            QVector<QPointF> points;

//            for(int i=size;i<oldPoints.count();++i){
//                points.append(QPointF(i-size ,oldPoints.at(i).y()));//替换数据用
//            }

//            qint64 sizePoints = points.count();
//            for(int k=0;k<size;++k){
//                points.append(QPointF(k+sizePoints,getData((((double)lastpointtime+k+1)/1000))));
//            }
//            left_series->replace(points);

//            lastpointtime = eltime;
//        }
    }
}


void MainWindow::on_emg1_clicked()
{
    left_emgroute=1;
    left_chart->setTitle("肌电通道1");
}

void MainWindow::on_emg2_clicked()
{
    left_emgroute=2;
    left_chart->setTitle("肌电通道2");
}

void MainWindow::on_emg3_clicked()
{
    left_emgroute=3;
    left_chart->setTitle("肌电通道3");
}

void MainWindow::on_emg4_clicked()
{
    left_emgroute=4;
    left_chart->setTitle("肌电通道4");
}

void MainWindow::on_emg5_clicked()
{
    right_emgroute=5;
    right_chart->setTitle("肌电通道5");
}

void MainWindow::on_emg6_clicked()
{
    right_emgroute=6;
    right_chart->setTitle("肌电通道6");
}

void MainWindow::on_emg7_clicked()
{
    right_emgroute=7;
    right_chart->setTitle("肌电通道7");
}

void MainWindow::on_emg8_clicked()
{
    right_emgroute=8;
    right_chart->setTitle("肌电通道8");
}

void MainWindow::on_pushButton_outputloc_clicked()
{
    QString filename = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "/home", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if(filename==NULL)
    {

    }else
    {
        ui->outputloc->setText(filename);
    }
}
