#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork/QTcpSocket>
#include "workthread.h"
#include <QtCharts>

#include <QWidget>
#include <QPen>
#include <QPainter>
#include <QtSvg/QSvgWidget>
#include <QtSvg/QSvgRenderer>
#include <QMouseEvent>

using namespace QtCharts;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
protected:
    void timerEvent(QTimerEvent *event) Q_DECL_OVERRIDE;
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
signals:
    void Send_IP(QString);
    void Send_Port(int);

private slots:
    void Displaywifidata_01(QString);
    void Displaywifidata_02(QString);

    void on_pushButton_Connect_clicked();
    void on_pushButton_Send_clicked();

    void on_pushButton_datasaving2_clicked();
    void on_pushButton_dataclear_1_clicked();
    void on_pushButton_dataclear_2_clicked();

    void on_emg1_clicked();

    void on_emg2_clicked();

    void on_emg3_clicked();

    void on_emg4_clicked();

    void on_emg6_clicked();

    void on_emg5_clicked();

    void on_emg7_clicked();

    void on_emg8_clicked();

    void on_pushButton_outputloc_clicked();

private:
    Ui::MainWindow *ui;
    WorkThread *thread1;
    double getData(double time);
};

#endif // MAINWINDOW_H
