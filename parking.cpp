/*******************************/
//停车场收费
//完成功能：显示车位信息   日期：2017年4月6日
//完成功能：入库操作      日期：2017年4月7日
//完成功能：查询界面直接跳转到入库界面    日期：2017年4月12日
//完成功能：出库操作                   日期：2017年4月20日

//数据库内容 序号0 卡号1 停车位编号2 车位状态3 车位类型4 停车日期5 停车时间6

/*******************************/
#include "ui_parking.h"
#include "parking.h"
#include "carport.h"

#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QTimer>
#include <QTimerEvent>
#include <QDateTime>
#include <QMessageBox>

static Parking* s_instance = NULL;
Parking *Parking::TheInstance()
{
    if(!s_instance)
    {
        s_instance = new Parking();
    }
    return s_instance;
}

Parking::Parking(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Parking)
{
    ui->setupUi(this);
    this->setWindowTitle("停车场收费系统");
    ParkWidget = ui->stackedWidget;
    ui->stackedWidget->setCurrentWidget(ui->pageMain);
    statusTime = new QLabel();
    statusTime->setAlignment(Qt::AlignHCenter);
    ui->statusbar->addWidget(statusTime);
    ui->statusbar->setStyleSheet(QString("QStatusBar::item{border: 0px}"));
    ui->lineEditCardIdEnter->setValidator(new QIntValidator(1000, 10000, this));
    //用于刷新系统时间
    timer = new QTimer(this);
    timer->start(1000);
    car = new carport();

    initCarPortSql();//初始化数据库

    vBoxOne = NULL;
    vBoxTwo = NULL;
    vBoxThree = NULL;
    vBoxFour = NULL;
    setInit();
    connect(timer, SIGNAL(timeout()), this, SLOT(updataTime()));
    connect(ui->pushButtonSureOut,&QPushButton::clicked,this,&Parking::outCarPort);
}

//刷新停车位界面数据
void Parking::setInit()
{
    setOne();
    setTwo();
    setThree();
    setFour();
}
Parking::~Parking()
{
    delete ui;
}

 //初始化 停车场数据库
int Parking::initCarPortSql()
{
    carPortDb = QSqlDatabase::addDatabase("QSQLITE");//添加sqlite数据
    carPortDb.setHostName("127.0.0.1");
    carPortDb.setPort(3306);
    carPortDb.setUserName("root");
    carPortDb.setPassword("12345");
    carPortDb.setDatabaseName("carPort.db");

    if(!carPortDb.open())
    {
        qDebug()<<"open database fail!"<<carPortDb.lastError().text()<<endl;
        return -1;
    }
    else
    {
         qDebug()<<"open database success!"<<endl;
    }

    QSqlQuery *query = new QSqlQuery;
    //数据库内容 序号 卡号 停车位编号 车位状态 车位类型 停车日期 停车时间
    bool success = query->exec("CREATE TABLE carPortInfo\
                               (Num int primary key,\
                               card_Id int unique,\
                               carPort_Id int not null unique,\
                               carPort_Status int,\
                               carPort_Type int not null,\
                               parking_Data int,\
                               parking_Time int\
                                )");
    if(success)
    {
        qDebug()<<"数据库表创建成功!"<<endl;
    }else
    {
        qDebug()<<"数据库表创建失败!"<<carPortDb.lastError().text()<<endl;
    }
    bool su = query->exec("insert into carPortInfo values\
                (1,1,1,1,1,20170101,121150),\
                (2,2,2,1,1,20170101,121150),\
                (3,3,3,1,1,20170101,121150)");
    qDebug()<<su<<carPortDb.lastError().text()<<endl;
//                (1,1,1,1,1,2017-01-01 12:11:50),\
//                (2,2,2,1,1,2017-01-01 12:11:50),\
    qDebug()<<carPortDb.lastError().text()<<endl;
    return 0;
}
//设置一号区停车位
int Parking::setOne()
{
    if(NULL != vBoxOne)      //如果一号区布局已经存在
    {
        delete vBoxOne;      //将已经存在的布局删除
        vBoxOne = NULL;      //因为布局为指针变量，释放内存后要置为安全状态
    }

    vBoxOne = new QVBoxLayout(ui->scrollAreaWidgetOne);//在滚动区域新建一个布局
    for(int i = 0;i < vecOne.size();i++)
    {
        delete vecOne[i];
    }
    vecOne.clear();
    QSqlQuery query;
    query.exec("select * from carPortInfo");
    while(query.next())
    {
        if(query.value(4).toInt() == 1)
        {
            carport *c = new carport;
            vecOne.append(c);
            c->setWidget(query.value(2).toString(),
                            query.value(1).toString(),
                            query.value(3).toString());
            vBoxOne->addWidget(c);
        }
    }
    return 0;
}
//设置二号区停车位
int Parking::setTwo()
{
    if(NULL != vBoxTwo)      //如果二号区布局已经存在
    {
        delete vBoxTwo;      //将已经存在的布局删除
        vBoxTwo = NULL;      //因为布局vBoxTwo为指针变量，释放内存后要置为安全状态
    }

    vBoxTwo = new QVBoxLayout(ui->scrollAreaWidgetTwo);//在滚动区域新建一个布局
    for(int i = 0;i < vecTwo.size();i++)
    {
        delete vecTwo[i];
    }
    vecTwo.clear();
    QSqlQuery query;
    query.exec("select * from carPortInfo");
    while(query.next())
    {
        if(query.value(3).toInt() == 2)
        {
            carport *c = new carport;
            vecTwo.append(c);
            c->setWidget(query.value(2).toString(),
                            query.value(1).toString(),
                            query.value(3).toString());
            vBoxTwo->addWidget(c);
        }
    }
    return 0;
}
//设置三号区停车位
int Parking::setThree()
{
    if(NULL != vBoxThree)      //如果三号区布局已经存在
    {
        delete vBoxThree;      //将已经存在的布局删除
        vBoxThree = NULL;      //因为布局vBoxThree为指针变量，释放内存后要置为安全状态
    }

    vBoxThree = new QVBoxLayout(ui->scrollAreaWidgetThree);//在滚动区域新建一个布局
    for(int i = 0;i < vecThree.size();i++)
    {
        delete vecThree[i];
    }
    vecThree.clear();
    QSqlQuery query;
    query.exec("select * from carPortInfo");
    while(query.next())
    {
        if(query.value(3).toInt() == 3)
        {
            carport *c = new carport;
            vecThree.append(c);
            c->setWidget(query.value(1).toString(),
                            query.value(0).toString(),
                            query.value(2).toString());
            vBoxThree->addWidget(c);
        }
    }
    return 0;
}
//设置四号区停车位
int Parking::setFour()
{
    if(NULL != vBoxFour)      //如果四号区布局已经存在
    {
        delete vBoxFour;      //将已经存在的布局删除
        vBoxFour = NULL;      //因为布局vBoxFour为指针变量，释放内存后要置为安全状态
    }

    vBoxFour = new QVBoxLayout(ui->scrollAreaWidgetFour);//在滚动区域新建一个布局
    for(int i = 0;i < vecFour.size();i++)
    {
        delete vecFour[i];
    }
    vecFour.clear();
    QSqlQuery query;
    query.exec("select * from carPortInfo");
    while(query.next())
    {
        if(query.value(3).toInt() == 4)
        {
            carport *c = new carport;
            vecFour.append(c);
            c->setWidget(query.value(1).toString(),
                            query.value(0).toString(),
                            query.value(2).toString());
            vBoxFour->addWidget(c);
        }
    }
    return 0;
}
//退出
void Parking::on_actionExit_triggered()
{
    this->close();
}
//跳转到入库界面
void Parking::on_actionEnter_triggered()
{
    ui->stackedWidget->setCurrentWidget(ui->pageEnter);

}

//跳转到出库界面
void Parking::on_actionOut_triggered()
{
    ui->stackedWidget->setCurrentWidget(ui->pageOut);
}
//跳转到系统信息（显示价格）
void Parking::on_actionSysInfo_triggered()
{
    ui->stackedWidget->setCurrentWidget(ui->pageSysInfo);
}
//主界面跳转到查询界面
void Parking::on_pushButtonCheck_clicked()
{
     ui->stackedWidget->setCurrentWidget(ui->pageCheck);

}
//主界面跳转到入库界面
void Parking::on_pushButtonEnter_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->pageEnter);
}
//主界面跳转到出库界面
void Parking::on_pushButtonOut_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->pageOut);
}
//主界面跳转到关于系统界面
void Parking::on_pushButtonSysInfo_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->pageSysInfo);
}
//刷新入库界面的时间
void Parking::updataTime()
{
    QDateTime dateTime = QDateTime::currentDateTime();
    int y=dateTime.date().year();
    int m=dateTime.date().month();
    int d=dateTime.date().day();
    QString strTime=dateTime.time().toString();
    //显示时间格式为例如 年/月/日 几点几分
    statusTime->setText(QString::number(y)+"/"+QString::number(m)+"/"+QString::number(d)+"  "+strTime);
    ui->labelSysTime->setText(QString::number(y)+"/"+QString::number(m)+"/"+QString::number(d)+"  "+strTime);
}
//入库界面确定按钮   入库操作
void Parking::on_pushButtonSure_clicked()
{
    //获取界面上的数据
    int CardId = ui->lineEditCardIdEnter->text().toInt();//卡号
    int CarPortId = ui->lineEditCarPortIdEnter->text().toInt();//停车位
    QDateTime dateTime = QDateTime::currentDateTime();//时间
    int parkingData = dateTime.date().year()*10000+dateTime.date().month()*100+dateTime.date().day();
    int parkingTime = dateTime.time().hour()*10000+dateTime.time().minute()*100+dateTime.time().second();

    qDebug()<<parkingData<<endl;
    qDebug()<<parkingTime<<endl;

    QSqlQuery query;
    query.exec("select * from carPortInfo");
    while(query.next())
    {
         //匹配数据库中的数据
        if(query.value(2) == CarPortId)
        {
            if(query.value(3) == 0)
            {
                QMessageBox::warning(this,"错误","车位已有车");
                return;
            }
            //处理入库事件
            QString cmd = tr("update carPortInfo set card_Id = '%1',\
                            carPort_Status = '0',parking_Data = '%2',\
                            parking_Time = '%3' where carPort_Id = '%4'")
                            .arg(CardId).arg(parkingData).arg(parkingTime).arg(CarPortId) ;

            query.exec(cmd);
            qDebug()<<query.value(0).toInt()<<endl;
            QMessageBox::information(this,"通知信息","停车成功");
       }

    }
    //刷新显示
    setInit();
    //跳转回查询界面
    ui->stackedWidget->setCurrentWidget(ui->pageCheck);
}
//入库界面返回按钮查询界面
void Parking::on_pushButton_2_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->pageCheck);
}
//出库
void Parking::outCarPort()
{
    //获取界面上的数据 卡号 时间
    int CardId = ui->lineEditCardIdOut->text().toInt();
//    int CarPortId = ui->lineEditCarPortIdEnter->text().toInt();
    QDateTime dateTime = QDateTime::currentDateTime();//时间
    int outData = dateTime.date().year()*10000+dateTime.date().month()*100+dateTime.date().day();
    int outTime = dateTime.time().hour()*10000+dateTime.time().minute()*100+dateTime.time().second();
    //查询数据库
    QSqlQuery query;
    query.exec("select * from carPortInfo");
    while(query.next())
    {
        //车位状态
        if(query.value(3) == 0)
        {
              //匹配数据库中的数据
            if(query.value(1) == CardId)
            {
                int CarPortId = query.value(1).toInt();
                int parkingData = query.value(5).toInt();//获取数据库中停车日期
                int parkingTime = query.value(6).toInt();//获取数据库中停车时间

                //计算停车时长
                int durationDataY = (outData - parkingData)/10000;//年
                int durationDataM = (outData - parkingData)/100;//月
                int durationDataD = (outData - parkingData);//日

                int durationTimeH = (outTime - parkingTime)/10000;//小时
                int durationTimeM = (outTime - parkingTime)/100;//分
                int durationTimeS = (outTime - parkingTime);//秒

                int parkingCost = durationTimeS + (durationTimeM * 60) + (durationTimeH *60 *60) +
                        (durationDataD * 60 *60 *24) + (durationDataM *60 *60 *24 *12) + (durationDataY *60 *60 *24 *12 *365);
                qDebug()<<parkingCost<<endl;
                qDebug()<<"进入"<<endl;
                //处理入库事件
                QString cmd = tr("update carPortInfo set carPort_Status = '1',Num = '%1' where  Num = '%2'")
                                .arg(CarPortId).arg(CardId);
                query.exec(cmd);
                QMessageBox::information(this,"通知信息","出库成功");

                //刷新显示
                setInit();
                //跳转回查询界面
                ui->stackedWidget->setCurrentWidget(ui->pageCheck);
                return;
            }
        }

    }
     QMessageBox::information(this,"通知信息","请输入正确卡号");

}

void Parking::carportParking(int &carportId)
{
    qDebug()<<"xiangying"<<endl;
    QString carNum = QString("%1").arg(carportId);
    setInit();
    ui->stackedWidget->setCurrentWidget(ui->pageEnter);
    qDebug()<<"tiaozhuan"<<endl;
    ui->lineEditCarPortIdEnter->setText(carNum);

}
void Parking::on_action_triggered()
{
    ui->stackedWidget->setCurrentWidget(ui->pageCheck);
}
