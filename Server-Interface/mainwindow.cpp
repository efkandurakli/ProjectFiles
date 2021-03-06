#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <fstream>
#include <QByteArray>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

//    t.start();

    qDebug() << " ı am in main thread";

    server = new QTcpServer(this);
    list = new QList<QTcpSocket*>;

    listPatient = new QList<Patient*>;
    listNursePatient = new QList<nursePatient*>;
    listUser = new QList<User*>;



    parserPatient();
    parserUser();
    parserNursePatient();

    mapper = new QSignalMapper(this);

    if(!server->listen(QHostAddress::Any,8081)){
        qDebug() <<"server not ok";

    }else{
        qDebug() <<"server ok";
    }
    connect(server, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));
    connect(mapper, SIGNAL(mapped(int)), this, SLOT(slotReadyRead(int)));

}

MainWindow::~MainWindow()
{
  //  t.wait();
  //  t.exit();
    server->close();
    delete list;
    delete ui;
}


void MainWindow::slotNewConnection()
{

    qWarning() << "newConnection";
    list->append(server->nextPendingConnection());

    //here you map each client to its number in the list
    mapper->setMapping(list->last(), list->length()-1);

    //here we say, that when ever a client from the QList sends readyRead() the mapper should be used
    //with the property (list->length()-1) defined in the line above
    connect(list->last(), SIGNAL(readyRead()), mapper, SLOT(map()));

    QString str = "5";

    list->last()->write(QString::number(onlineD+1).toUtf8());

}

void MainWindow::slotReadyRead(int index)
{


  //  int processing;
    QString process;

   // QByteArray template1;

    process =  list->at(index)->readAll();

    qDebug() << process;
   // processing = (tr(template1)).toInt();


    if(process == "1"){
        char d= 'D';
        char n='H';
        char sendcontrol = '0';
        bool control = true;
        QString strInf;
        QString hastaind;
        QString hastaind1;
        qDebug() << "ses icine girdi";


        list->at(index)->waitForReadyRead(10000);

        strInf =  list->at(index)->readAll();

        // to do parser

        qDebug() << strInf;

        if(strInf.split(" ").size() > 2 ){
            for(int i = 0 ; i < listUser->size() ; i++){

                qDebug() <<"asdasds  " <<strInf.split(" ").at(1);
                if(strInf.split(" ").at(1) == listUser->at(i)->getName()){

                    qDebug() <<"asdasds  " <<strInf.split(" ").at(1);
                    if(listUser->at(i)->getJob() == "Doctor"){
                         onlineD++;
                        qDebug() << "Girdi";
                        send( list->at(index)->socketDescriptor(),&d,sizeof(char),0);

                        list->at(index)->waitForReadyRead(2000);
                        int ind;
                        hastaind = list->at(index)->readAll();
                        ind = hastaind.toInt();
                        ind = ind % listPatient->size();

                        qDebug() << "gelen index" <<ind;
                        if(ind < 0)
                            ind += listPatient->size();

                        QString strtemp= listPatient->at(ind)->stringfunction();
                        send( list->at(index)->socketDescriptor(),strtemp.toUtf8(),strtemp.size(),0);
                        control = false;
                        break;
                    }else if(listUser->at(i)->getJob() == "Hemsire"){
                         onlineH++;
                        send( list->at(index)->socketDescriptor(),&n,sizeof(char),0);
                        list->at(index)->waitForReadyRead(2000);
                        int ind1;
                        hastaind1 = list->at(index)->readAll();
                        ind1 = hastaind1.toInt();
                        ind1 = ind1 % listNursePatient->size();

                        qDebug() << "gelen index" <<ind1;
                        if(ind1 < 0)
                            ind1 += listNursePatient->size();
                        QString strtemp1= listNursePatient->at(ind1)->stringfunction();
                        send( list->at(index)->socketDescriptor(),strtemp1.toUtf8(),strtemp1.size(),0);

                        control = false;
                        break;
                    }
                }
            }
        }
        if(control ){

            send( list->at(index)->socketDescriptor(),&sendcontrol,sizeof(char),0);
        }
    }

    else if(process == "2" ){

        //gelen sayiya göre

        int hastaindexi;
        qDebug() << "2 ye girdi";

        list->at(index)->waitForReadyRead(1000);
        hastaindexi = (tr(list->at(index)->readAll())).toInt();
        hastaindexi = hastaindexi%listPatient->size();

        if(hastaindexi < 0){
            hastaindexi += listPatient->size();
        }

        QString st= listPatient->at(hastaindexi)->stringfunction();

        send( list->at(index)->socketDescriptor(),st.toUtf8(),st.size(),0);


    }


    else if(process == "8"){

        QString nurseindex;
        int hastaindexiNurse;

        qDebug() << "10 a girdi";
        // hemsirenin hasta bilgilerini alacagi yer
        list->at(index)->waitForReadyRead(1000);

        nurseindex = (list->at(index)->readAll());
        hastaindexiNurse = nurseindex.toInt();
        hastaindexiNurse = hastaindexiNurse%listNursePatient->size();
        if(hastaindexiNurse < 0){
            hastaindexiNurse += listNursePatient->size();
        }
        QString st1= listNursePatient->at(hastaindexiNurse)->stringfunction();
qDebug() << st1;
        send( list->at(index)->socketDescriptor(),st1.toUtf8(),st1.size(),0);
    }




}
void MainWindow::parserPatient()
{
    ifstream file("/home/akilok/MytestingServerInterface/patients.csv");

    QString temp;
    string temp1;
    QStringList listStr;

    qDebug() << "FILE ";

    getline(file,temp1);
    while(getline(file,temp1)){

        qDebug() << temp1.c_str();
        temp = temp1.c_str();
        listStr = temp.split(',');

        listPatient->append(new Patient(listStr.at(0),listStr.at(1),listStr.at(2),listStr.at(3),listStr.at(4),
                                        listStr.at(5),listStr.at(6),listStr.at(7)));
    }




}
void MainWindow::parserNursePatient()
{
    ifstream file("/home/akilok/MytestingServerInterface/nursePatient.csv");

    QString temp;
    string temp1;
    QStringList listStr;

    qDebug() << "FILE ";

    getline(file,temp1);
    while(getline(file,temp1)){

        qDebug() << temp1.c_str();
        temp = temp1.c_str();
        listStr = temp.split(',');

        listNursePatient->append(new nursePatient(listStr.at(0),listStr.at(1),listStr.at(2),listStr.at(3),listStr.at(4),
                                        listStr.at(5),listStr.at(6),listStr.at(7),listStr.at(8)));
    }




}

void MainWindow::parserUser()
{
    ifstream file("/home/akilok/MytestingServerInterface/users.csv");

    QString temp;
    string temp1;
    QStringList listStr;

    qDebug() << "FILE ";

    getline(file,temp1);
    while(getline(file,temp1)){

        qDebug() << temp1.c_str();
        temp = temp1.c_str();
        listStr = temp.split(',');
        qDebug() << listStr.size();
        listUser->append(new User(listStr.at(0),listStr.at(1),listStr.at(2),listStr.at(3)));
    }




}

void MainWindow::on_pushButton_clicked()
{
    qDebug() << "butona basildi\n";
}
