#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "log.h"

#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    LOG_INFO("Hello, mainWindow");
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    LOG_INFO("~MainWindow()");
    delete ui;
}

void MainWindow::tcpClientDisconnected() {
    LOG_INFO("TcpClient disconnected");
    ui->connectPushButton->setText("Connect");
    ui->socketProtocol->setDisabled(false);
    ui->clientTypeComboBox->setDisabled(false);
    ui->hostLineEdit->setDisabled(false);
    ui->portSpinBox->setDisabled(false);
    ui->sendPushButton->setDisabled(true);
    LOG_INFO("Disconnect tcpClient");
    ui->recvTextBrowser->append("!!!TcpClient disconnected!!!\r\n\r\n");
}

void MainWindow::tcpClientReceiveData() {
    QByteArray buffer;
    //读取缓冲区数据
    buffer = m_pTcpSocket->readAll();
    LOG_HEX(buffer.data(), buffer.size());
    ui->recvTextBrowser->append(QString("Receive: ") + QString::fromUtf8(buffer));
}

void MainWindow::tcpServerDisconnected() {
    LOG_INFO("TcpServer disconnected");
    ui->recvTextBrowser->append("!!!TcpServer disconnected!!!\r\n\r\n");
    if (ui->connectPushButton->text() == "Disconnect") {
        if (tcpServerConnect() == 0) {
            ui->connectPushButton->setText("Wait");
            return;
        } else {
            LOG_ERROR("Listen error");
        }
    }
    ui->connectPushButton->setText("Listen");
    ui->socketProtocol->setEnabled(true);
    ui->clientTypeComboBox->setEnabled(true);
    ui->hostLineEdit->setEnabled(false);
    ui->portSpinBox->setEnabled(true);
    ui->sendPushButton->setEnabled(false);
}

void MainWindow::tcpServerReceiveData() {
    QByteArray buffer;
    //读取缓冲区数据
    buffer = m_pTcpServerSocket->readAll();
    LOG_HEX(buffer.data(), buffer.size());
    ui->recvTextBrowser->append(QString("Receive: ") + QString::fromUtf8(buffer));
}

void MainWindow::tcpServerNewConnect() {
    m_pTcpServerSocket = m_pTcpServer->nextPendingConnection();
    m_pTcpServer->close();
    QObject::connect(m_pTcpServerSocket, SIGNAL(disconnected()), this, SLOT(tcpServerDisconnected()));
    QObject::connect(m_pTcpServerSocket, SIGNAL(readyRead()),this, SLOT(tcpServerReceiveData()));
    ui->connectPushButton->setText("Disconnect");
    ui->recvTextBrowser->append(QString("!!!TcpServer connect to %1:%2; Stop Listen!!!\r\n\r\n").arg(getIpv4Address(m_pTcpServerSocket->peerAddress().toIPv4Address())).arg(m_pTcpServerSocket->peerPort()));
}

int MainWindow::tcpClientConnect() {
    QString host = ui->hostLineEdit->text();
    if (host.isEmpty()) {
        LOG_ERROR("Host is empty");
        return -1;
    }

    int port = ui->portSpinBox->value();
    LOG_INFO("Connect to %s:%d", host.toLatin1().data(), port);

    if (m_pTcpSocket == nullptr) {
        LOG_WARN("TcpSocket is null");
        m_pTcpSocket = std::make_shared<QTcpSocket>();
        QObject::connect(m_pTcpSocket.get(), SIGNAL(disconnected()), this, SLOT(tcpClientDisconnected()));
        QObject::connect(m_pTcpSocket.get(), SIGNAL(readyRead()), this, SLOT(tcpClientReceiveData()));
    } else {
        m_pTcpSocket->abort();
    }

    m_pTcpSocket->connectToHost(host, port);

    if (!m_pTcpSocket->waitForConnected(5000)) {
        LOG_ERROR("Connect error");
        QMessageBox::critical(this, "Connect error", QString("Can't connect to %1:%2").arg(host).arg(port));
        return -1;
    }

    LOG_INFO("Connect success");
    ui->recvTextBrowser->append(QString("!!!Connect to %1:%2 success!!!\r\n").arg(host).arg(port));
    return 0;
}

int MainWindow::tcpServerConnect() {
    int port = ui->portSpinBox->value();
    LOG_INFO("Listen to port %d", port);

    if (m_pTcpServer == nullptr) {
        LOG_WARN("TcpServer is null");
        m_pTcpServer = std::make_shared<QTcpServer>();
        QObject::connect(m_pTcpServer.get(), SIGNAL(newConnection()), this, SLOT(tcpServerNewConnect()));
    } else if (m_pTcpServer->isListening()){
        LOG_INFO("Close last listen");
        m_pTcpServer->close();
    }

    if (!m_pTcpServer->listen(QHostAddress::Any, port)) {
        LOG_ERROR("Open port %d failed", port);
        QMessageBox::critical(this, "Listen failed", QString("Listen to port %1 failed").arg(port));
        return -1;
    }

    LOG_INFO("Listen port %d success", port);
    ui->recvTextBrowser->append(QString("!!!Begin listen to port %1!!!\r\n").arg(port));
    return 0;
}

QString MainWindow::getIpv4Address(unsigned int ip) {
    unsigned char d = ip & 0xff;
    unsigned char c = (ip >> 8) & 0xff;
    unsigned char b = (ip >> 16) & 0xff;
    unsigned char a = (ip >> 24) & 0xff;
    return QString().sprintf("%d.%d.%d.%d", a, b, c, d);
}

void MainWindow::on_clearRecvPushButton_clicked()
{
    ui->recvTextBrowser->clear();
}

void MainWindow::on_clearSendPushButton_clicked()
{
    ui->sendTextEdit->clear();
}

void MainWindow::on_clientTypeComboBox_currentIndexChanged(int index)
{
    LOG_INFO("ClientTypeComboBox currentIndex changed to %d", index);
    switch (index) {
    case 0:
        ui->connectPushButton->setText("connect");
        ui->hostLineEdit->setEnabled(true);
        break;

    case 1:
        ui->connectPushButton->setText("Listen");
        ui->hostLineEdit->setEnabled(false);
        break;

    default:
        LOG_ERROR("Invalid index");
        QMessageBox::critical(this, "Invalid param", QString("Index %1 is invalid").arg(index));
        ui->clientTypeComboBox->setCurrentIndex(0);
        break;
    }
}

void MainWindow::on_socketProtocol_currentIndexChanged(int index)
{
    LOG_INFO("SocketProtocol currentIndex changed to %d", index);
    switch (index) {
    case 0:
        //        ui->connectPushButton->setText("connect");
        break;

        //    case 1:
        //        ui->connectPushButton->setText("Listen");
        //        break;

    default:
        LOG_ERROR("Invalid index");
        QMessageBox::critical(this, "Invalid param", QString("Index %1 is invalid").arg(index));
        ui->socketProtocol->setCurrentIndex(0);
        break;
    }
}

void MainWindow::on_connectPushButton_clicked()
{
    switch (ui->socketProtocol->currentIndex()) {
    case 0:
        if (ui->clientTypeComboBox->currentIndex() == 0) {
            LOG_INFO("Tcp client");
            if (ui->connectPushButton->text() == "Disconnect") {
                m_pTcpSocket->disconnectFromHost();
                ui->connectPushButton->setText("Connect");
                ui->socketProtocol->setDisabled(false);
                ui->clientTypeComboBox->setDisabled(false);
                ui->hostLineEdit->setDisabled(false);
                ui->portSpinBox->setDisabled(false);
                ui->sendPushButton->setDisabled(true);
                LOG_INFO("Disconnect tcpClient");
            } else {
                if (tcpClientConnect() == 0) {
                    ui->connectPushButton->setText("Disconnect");
                    ui->socketProtocol->setDisabled(true);
                    ui->clientTypeComboBox->setDisabled(true);
                    ui->hostLineEdit->setDisabled(true);
                    ui->portSpinBox->setDisabled(true);
                    ui->sendPushButton->setDisabled(false);
                    LOG_INFO("TcpClient connect");
                }
            }
        } else {
            LOG_INFO("Tcp server");
            if (ui->connectPushButton->text() != "Listen") {
                if (m_pTcpServerSocket != nullptr) {
                    m_pTcpServerSocket->abort();
                }
                m_pTcpServer->close();
                ui->connectPushButton->setText("Listen");
                ui->socketProtocol->setDisabled(false);
                ui->clientTypeComboBox->setDisabled(false);
                ui->hostLineEdit->setEnabled(false);
                ui->portSpinBox->setDisabled(false);
                ui->sendPushButton->setDisabled(true);

                ui->recvTextBrowser->append("!!!Stop server!!!\r\n\r\n");
                LOG_INFO("Stop server");
            } else {
                if (tcpServerConnect() == 0) {
                    ui->connectPushButton->setText("Wait");
                    ui->socketProtocol->setDisabled(true);
                    ui->clientTypeComboBox->setDisabled(true);
                    ui->hostLineEdit->setEnabled(false);
                    ui->portSpinBox->setDisabled(true);
                    ui->sendPushButton->setDisabled(false);
                    LOG_INFO("Begin listen");
                }
            }
        }
        break;

    case 1:
        break;

    default:
        break;
    }
}

void MainWindow::on_sendPushButton_clicked()
{
    QString content = ui->sendTextEdit->toPlainText();
    QByteArray sendData = content.toUtf8();
    switch(ui->socketProtocol->currentIndex()) {
    case 0:
        if (ui->clientTypeComboBox->currentIndex() == 0) {
            if (m_pTcpSocket == nullptr) {
                LOG_ERROR("TcpSocket is null");
                break;
            } else {
                m_pTcpSocket->write(sendData.data(), sendData.size());
                m_pTcpSocket->flush();
                LOG_DEBUG("socket send:");
                LOG_HEX(sendData.data(), sendData.size());
                ui->recvTextBrowser->append(QString("Send: ") + content);
            }
        } else {
            if (m_pTcpServerSocket == nullptr) {
                LOG_ERROR("TcpServerSocket is null");
                break;
            } else {
                m_pTcpServerSocket->write(sendData.data(), sendData.size());
                m_pTcpServerSocket->flush();
                LOG_DEBUG("Server socket send:");
                LOG_HEX(sendData.data(), sendData.size());
                ui->recvTextBrowser->append(QString("Send: ") + content);
            }
        }

        break;

    case 1:
        break;

    default:
        LOG_ERROR("Invalid protocol %d", ui->socketProtocol->currentIndex());
        QMessageBox::critical(this, "Invalid protocol", QString("Index %1 is invalid").arg(ui->socketProtocol->currentIndex()));
        break;
    }
}
