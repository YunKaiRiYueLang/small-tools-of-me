
#include "comdisp.h"
#include"win-api.h"
#include<qdebug.h>
comdisp::comdisp(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    setWindowOpacity(0.8);
    centerLabel=new QLabel;
    setCentralWidget(centerLabel);
    setGeometry(1600,30,300,50);
    setFixedWidth(300);
    setFixedHeight(50);
    asi.RemoveAll();
    EnumSerialPorts(asi, FALSE/*include all*/);
    std::vector<std::string> list_port;
    list_port.clear();
    std::string str;
    for (int ii = 0; ii < asi.GetSize(); ii++) {
        std::string str_tem = CT2A(asi[ii].strFriendlyName.GetBuffer());
        list_port.push_back(str_tem);
        str.append(list_port[ii]);
    }
    centerLabel->setText(QString::fromLocal8Bit(str.c_str()));
    
    //时钟显示再最前面。
    //再最佳一个定时器。
    setWindowFlag(Qt::WindowStaysOnTopHint);
    setWindowFlag(Qt::FramelessWindowHint);
    timer=new QTimer;
    connect(timer,SIGNAL(timeout()),this,SLOT(enumPort()));
    timer->start(1000);//每隔1秒钟，触发扫描端口。
}
void comdisp::enumPort() {
    //qDebug()<<__FUNCTION__;
    asi.RemoveAll();
    EnumSerialPorts(asi, FALSE/*include all*/);
    std::string str;
    for (int ii = 0; ii < asi.GetSize(); ii++) {
        std::string str_tem = CT2A(asi[ii].strFriendlyName.GetBuffer());
        str.append(str_tem);
    }
    centerLabel->setText(QString::fromLocal8Bit(str.c_str()));
    
}
