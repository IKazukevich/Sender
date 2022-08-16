#include "server.h"


Server::Server()
{
}

void Server::startServer()
{
	QString IP;
	const QHostAddress &localhost = QHostAddress(QHostAddress::LocalHost);
	for (const QHostAddress &address: QNetworkInterface::allAddresses()) {
		if (address.protocol() == QAbstractSocket::IPv4Protocol && address != localhost)
		{
			IP = address.toString();
			break;
		}
	}
	if(!this->isListening()){
		if(this->listen(QHostAddress(IP), 2323)){
			emit sendStatus("Server started. IP: " + IP);
		}else{
			emit sendStatus("Error occured.");
		}
	}else{
		emit sendStatus("Server is already on.");
	}
}

void Server::stoppingListening()
{
	if(this->isListening()){
		this->close();
		emit sendStatus("Server stopped listening.");	// Отправка файлов на один и тот же компьютер
	}													// возможна используя два открытых приложения
}														// но не одно единстенное для передачи и приема


void Server::incomingConnection(qintptr socketDescriptor)
{
	socket = new QTcpSocket;
	socket->setSocketDescriptor(socketDescriptor);
	emit sendStatus("Sender connected. Descriptor:  " + QString::number(socketDescriptor));

	connect(socket, &QTcpSocket::readyRead, this, &Server::slotReadyReadName);
	connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
}

void Server::slotReadyRead()
{
	disconnect(socket, &QTcpSocket::readyRead, this, &Server::slotReadyRead);
	data.open(QFile::WriteOnly | QFile::Append);
	while(receivedSize < fileSize){
		if(socket->bytesAvailable()==0)socket->waitForReadyRead(100);
		emit progressChanged((int)((double)receivedSize/fileSize*100)+1);
		QApplication::processEvents();
		if(!socket->isValid()){
			emit sendStatus("Error occured!");
			return;
		}
		QByteArray chunkOfFile = socket->read(1024*8);
		receivedSize += chunkOfFile.size();
		data.write(chunkOfFile);
		chunkOfFile.clear();
	}
	receivedSize = 0;
	socket->close();
	socket = nullptr;
	emit progressChanged(0);
	emit sendStatus("Download complete!");
	data.close();
}

void Server::slotReadyReadName()
{
	disconnect(socket, &QTcpSocket::readyRead, this, &Server::slotReadyReadName);
	qint64 FileNameSize;
	QByteArray fileName;
	while(socket->bytesAvailable() < (qint64)sizeof(fileSize)){}
	socket->read((char*)&fileSize, sizeof(fileSize));
	receivedSize = 0;
	while(socket->bytesAvailable() < (qint64)sizeof(fileSize)){}
	socket->read((char*)&FileNameSize, sizeof(FileNameSize));
	while(socket->bytesAvailable() < FileNameSize){}
	fileName = socket->read(FileNameSize);

	data.setFileName(fileName);
	connect(socket, &QTcpSocket::readyRead, this, &Server::slotReadyRead);
	emit sendStatus(QString::number(FileNameSize));
	emit sendStatus(QString("Getting file: ") + fileName);
	emit sendStatus("Size of file: " + QString::number(fileSize) + " bytes");

}

void Server::ErrorOccured(QString msg)
{
	emit sendStatus(msg);
	socket->close();
}
