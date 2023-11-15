#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QAction>
#include <QLabel>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QVBoxLayout *vl =  new QVBoxLayout;

    this->tb = new QTextBrowser(this);
    vl->addWidget(this->tb);

    QHBoxLayout *hl = new QHBoxLayout;
    this->leText = new QLineEdit(this);
    this->pbSend = new QPushButton("Enviar", this);
    hl->addWidget(this->leText);
    hl->addWidget(this->pbSend);
    vl->addLayout(hl);

    connect(this->pbSend, &QPushButton::clicked, this, &MainWindow::onSendMessage);
    this->ui->centralwidget->setLayout(vl);

    this->ui->toolBar->addWidget(new QLabel("Puerto:", this));

    this->cbPort = new QComboBox(this);
    this->ui->toolBar->addWidget(this->cbPort);

    this->ui->toolBar->addSeparator();

    QAction *act = new QAction(this);
    act->setText("Conectar");
    this->ui->toolBar->addAction(act);
    connect(act, &QAction::triggered, this, &MainWindow::onOpenClosePort);

    this->server = new QTcpServer(this);
    connect(this->server, &QTcpServer::newConnection, this, &MainWindow::onNewConnection);

    this->cbPort->setFixedWidth(150);

    this->cbPort->addItem("1234");
    this->cbPort->setEditable(true);

    this->pbSend->setEnabled(false);

    connect(this->leText, &QLineEdit::returnPressed, this, &MainWindow::onSendMessage);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onNewMessage()
{
    for (QTcpSocket *clientSocket : clients) {
        if (clientSocket->bytesAvailable() > 0) {
            QString message = clientSocket->readAll();
            qDebug() << "Received from client:" << message;
            this->tb->append(QString("Cliente " + QString::number(clientSocket->socketDescriptor()) +  ": " + message));
        }
    }
}

void MainWindow::onOpenClosePort()
{
    QAction *act = static_cast<QAction*>(sender());
    if (this->server->isListening()) {
        this->server->close();
        this->ui->statusbar->showMessage("Puerto cerrado");
        this->pbSend->setEnabled(false);
        act->setText("Abrir");
    }
    else {
        if(!this->server->listen(QHostAddress::LocalHost, this->cbPort->currentText().toUInt())){
            this->server->errorString();
            this->ui->statusbar->showMessage("No se pudo abrir el puerto");
        }

        this->ui->statusbar->showMessage("Server listening on port 1234");
        act->setText("Cerrar");
        this->pbSend->setEnabled(true);
    }
}

void MainWindow::onSendMessage()
{
    QTcpSocket *clientSocket;
    QString t = this->leText->text();
    qDebug() << "Data: " << t;
    if(!t.isEmpty()){
        this->tb->append(QString("Server: %1").arg(t));
        this->leText->clear();
        for (QTcpSocket *clientSocket : clients) {
            if(clientSocket->write(t.toUtf8()) == -1){
                this->ui->statusbar->showMessage("Error sending data");
            }
        }
    }
}

void MainWindow::onNewConnection(){
    QTcpSocket *clientSocket = this->server->nextPendingConnection();
    if(!clientSocket){
        qDebug() << "Couldn't obtain client socket";
    }
    else{
        this->ui->statusbar->showMessage("Client connected. Socket descriptor: " + QString::number(clientSocket->socketDescriptor()));
        qDebug() << "Client connected. Socket descriptor: " + QString::number(clientSocket->socketDescriptor());

        clients.append(clientSocket);
        connect(clientSocket, &QTcpSocket::readyRead, this, &MainWindow::onNewMessage);
    }
}
