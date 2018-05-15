//use QT_perfusion.ino file

#include "MainWindow.h"
#include <QTimer>
#include<QDebug>
#include "serial.h"
#include <QMouseEvent>
#include <QPushButton>
#include "QSignalMapper"

//#include "vars.h"
int bufShowSize=3000;
int nodes_N=340;
int lines_N=5;
float f;

QString run_str("pony_runs.png);");
QString dead("pony_dead.png);");
QString strh("pony_neitral.png);");
QString neitral("pony_neitral.png);");
QString stand_str("background-color: white ;background-repeat: no-repeat;background-position: right; background-image: url(release/");

Serial hSerial;
QPushButton* ON_BTN;
QPushButton* return_BTN;
QTimer *timer;
QTimer *run_timer;
QString qstr;
bool on=1;
uint8_t sendVal=1;
//work* WK;


void whitening(QWidget* w);
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);


    QSignalMapper* signalMapper = new QSignalMapper(this);
    connect(signalMapper, SIGNAL(mapped(int)),
            this, SLOT(buttonClicked(int)));

    ON_BTN=new QPushButton("on");


    connect(ON_BTN, SIGNAL(clicked()),
            signalMapper,         SLOT(map()));
    signalMapper->setMapping(ON_BTN, 4);

    return_BTN=new QPushButton("return");


    connect(return_BTN, SIGNAL(clicked()),
            signalMapper,         SLOT(map()));
    signalMapper->setMapping(return_BTN, 6);

    LE_COM=new QLineEdit;
    LE_speed=new QLineEdit;
    LE_speed->setText(QString("Fluid flow [mkl/sec]: 0.05"));

    qstr=QString("COM8");
    LE_COM->setText(qstr);

    int frame_width=4;
    QGridLayout* GL=new QGridLayout();
    GL->addWidget(LE_COM,0/frame_width,0%frame_width);
    GL->addWidget(LE_speed,0,1);
    GL->addWidget(ON_BTN,1,0);
    GL->addWidget(return_BTN,1,1);
    //    GL->setRowMinimumHeight(2,400);
    GL->setColumnMinimumWidth(2,160);
    ON_BTN->setMinimumWidth(60);
    ON_BTN->setMaximumWidth(60);
    ON_BTN->setMinimumHeight(60);
    return_BTN->setMaximumWidth(70);
    return_BTN->setMinimumHeight(60);
    LE_speed->setMinimumWidth(245);
    LE_COM->setMaximumWidth(120);
    //    ON_BTN->setStyleSheet();

    QWidget *centralWidget1=new QWidget();
    centralWidget1->setLayout(GL);
    setCentralWidget(centralWidget1);


    timer=new QTimer(this);

    connect(LE_speed,SIGNAL(returnPressed()),this,SLOT(setSpeed()));
    connect(ON_BTN,SIGNAL(released()),this,SLOT(turn()));
    connect(this,SIGNAL( btn_sig(int)),this,SLOT(btn_slot(int)));
    connect(LE_COM,SIGNAL(returnPressed()),this,SLOT(waitCOM_Subm()));
    //    connect();
    timer->start(100);

    run_timer=new QTimer;
    connect(run_timer,SIGNAL(timeout()),this,SLOT(change_pict()));
    this->update();

    setSpeed();

    //http://doc.qt.io/archives/qt-4.8/stylesheet-reference.html
    //http://doc.qt.io/archives/qt-4.8/stylesheet-reference.html#repeat
    setStyleSheet(stand_str+neitral);

    whitening(ON_BTN);
    ON_BTN->setStyleSheet("background-image: url(release/red.png); font-size: 20px;");
    whitening(return_BTN);
    whitening(LE_speed);
    LE_speed->setStyleSheet("background-image: url(release/empty.png); font-size: 16px;");
    whitening(LE_COM);
    LE_COM->setStyleSheet("background-image: url(release/empty.png); font-size: 16px;");

    //        QPixmap bkgnd("C:/Users/chibi/Documents/Lightshot/empty.png)");
    //        bkgnd = bkgnd.scaled(this->size(), Qt::IgnoreAspectRatio);
    //        QPalette palette;
    //        palette.setBrush(QPalette::Background, bkgnd);
    //        this->setPalette(palette);
}

void MainWindow::buttonClicked(int i)
{
    switch(i)
    {
    case 4:

        on=!on;
        if(on)
        {
            ON_BTN->setText("on");
            hSerial.write((uint8_t)(40));
            setStyleSheet(stand_str+(strh=neitral));
        }
        else
        {
            ON_BTN->setText("off");
            hSerial.write((uint8_t)(4));
            setStyleSheet(stand_str+(strh=dead));
        }
        break;
    case 6:
        hSerial.write((uint8_t)(6));
        setStyleSheet(stand_str+run_str);
        run_timer->start(4000);
        break;
    }
}

void MainWindow::drawing()
{
    this->update();
}



void MainWindow::setSpeed()
{
    QString h=LE_speed->text();
    h.resize(21);
    if(h!=QString("Fluid flow [mkl/sec]:"))
        LE_speed->setText(QString("Fluid flow [mkl/sec]: 0.05"));
    //    qDebug()<<QString::number(0.1);
    speed=LE_speed->text().remove(0,21).toFloat();
    if(speed==0) speed=0.0005;
    //    timer->start(100);
    float k=250;
    int I=k/speed;
    if(I==0)I=1;
    float s2=k/I;
    LE_speed->setText(LE_speed->text().remove(22,40)+QString::number(s2));
    timer->setInterval(I);
}

void MainWindow::waitCOM_Subm()
{
    qstr=LE_COM->text();
    std::string str1=qstr.toUtf8().constData();
    std::wstring str(str1.begin(),str1.end());
    hSerial.InitCOM(str.c_str());//was L"COM5"

    connect(timer, SIGNAL(timeout()), this, SLOT(send()));
    hSerial.write((uint8_t)(40));
    LE_COM->setDisabled(true);
    //    buttonClicked(4);
}

void MainWindow::send()
{

    //    if(on)
    {

        if(sendVal==1)
            sendVal=0;
        else
            sendVal=1;
        qDebug()<<"hello";
        hSerial.write(sendVal);
    }
}

void MainWindow::mousePressEvent(QMouseEvent *)
{

}

//void MainWindow::mainCircLE_COM()
//{



//}

void MainWindow::paintEvent(QPaintEvent* e)
{
    //    static float t=1;
    //    t+=.06;
    //    if(t>10)t=10;
    //    for (int i=0;i<t;i++)
    //        mainCircLE_COM();

    //    QPainter* painter=new QPainter(this);
    //    painter->setRenderHint(QPainter::Antialiasing, 1);
    //    QPen pen(Qt::black, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    //    painter->setPen(pen);
    //    painter->scaLE_COM(1.5,1.5);



    //    deLE_COMte painter;
}

MainWindow::~MainWindow()
{

}
void MainWindow::change_pict()
{
    setStyleSheet(stand_str+(strh));
}

void whitening(QWidget* w)
{
    w->setStyleSheet("font-size: 20px; background-image: url(release/empty.png);") ;

}
