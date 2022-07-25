#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_comdisp.h"
#include<qlabel.h>
#include<vector>
#include<string>
#include<qtimer.h>
#define _AFXDLL
#include<afxtempl.h>
#include<Windows.h>
#include"win-api.h"
class comdisp : public QMainWindow
{
    Q_OBJECT

public:
    comdisp(QWidget *parent = Q_NULLPTR);

private:
    Ui::comdispClass ui;
    QLabel* centerLabel;
    QTimer *timer;
    CArray<SSerInfo, SSerInfo&> asi;


private slots:
    void enumPort();
};
