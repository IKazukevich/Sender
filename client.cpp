#include "client.h"
#include "qcoreapplication.h"

Client::Client(QObject *parent)
	: QObject{parent}
{
	port = 2323;
	IP = "";
	fileAquired = false;
	ipAquired = false;
	socket = nullptr;
}

void Client::setFilename(const QString &newFilename) //
{
	fileAquired = true;
	FileForSending.setFileName(newFilename);
	emit sendStatus("File chosen: " + newFilename);
}

void Client::setIP(const QString &newIP)
{
	QString oIpRange = "(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])";
	QRegularExpression oIpRegex ("^" + oIpRange + "\\." + oIpRange + "\\." + oIpRange + "\\." + oIpRange + "$");
	QRegularExpressionMatch match = oIpRegex.match(newIP);
	if(match.hasMatch()){
		emit sendStatus("Reciever IP upated to: " + newIP);
		IP = newIP;
		ipAquired = true;
	}else{
		emit sendStatus("Wrong input. Input IP as following: XXX.XXX.XXX.XXX");
	}
}

void Client::SendFile()
{
	if(socket == nullptr){
		if(fileAquired && ipAquired){
			socket = new QTcpSocket();
			socket->connectToHost(QHostAddress(IP), port);
			emit sendStatus("Trying to connect.");
			socket->waitForConnected(3000);
			if(socket->state() == QTcpSocket::ConnectedState){
				emit sendStatus("Connected.");
				ContinueSending();
			}else{
				emit sendStatus("Could not connect.");
			}
		}else{
			if(!fileAquired) emit sendStatus("Choose a file to send");
			if(!ipAquired) emit sendStatus("Choose an ip to send");
		}
	}else{
		emit sendStatus("Already connected.");
	}
}

void Client::ContinueSending() {
	if(FileForSending.open(QFile::ReadOnly)){
		QByteArray data;
		QDataStream out(&data, QIODevice::WriteOnly);
		if(socket->write(data)==-1)ErrorOccured("Could not write data on that socket!");
		socket->waitForBytesWritten();
		connect(socket, SIGNAL(bytesWritten(qint64)), this, SLOT(sendPartOfFile()));
		sendNameOfFile(FileForSending);
		sendSize = 0;
		sendPartOfFile();
	} else{
		ErrorOccured("Could not open that file!");
		return;
	}
}

void Client::sendNameOfFile(QFile & FileForSending)
{
	qint64 ErrCheck;
	qint64 FileSize = FileForSending.size();
	QByteArray filename = QFileInfo(FileForSending).fileName().toUtf8();
	qint64 FilenameSize = filename.size();
	ErrCheck = socket->write((char*)&FileSize, sizeof(FileSize));
	if(ErrCheck == -1)ErrorOccured("Data error occured!");
	ErrCheck = socket->write((char*)&FilenameSize, sizeof(FilenameSize));
	if(ErrCheck == -1)ErrorOccured("Data error occured!");
	ErrCheck = socket->write(filename);
	if(ErrCheck == -1)ErrorOccured("Data error occured!");
}

void Client::ErrorOccured(QString msg)
{
	emit sendStatus(msg);
	fileAquired = false;
	socket->close();
	socket->deleteLater();
	socket = nullptr;
}

void Client::sendPartOfFile() {
	if(!FileForSending.atEnd()){
		QByteArray block = FileForSending.read(1024 * 8);
		sendSize+=block.size();
		emit progressChanged((int)((double)sendSize/FileForSending.size())+1);
		QApplication::processEvents();
		qint64 send = socket->write(block, block.size());
		if(send == -1)ErrorOccured("Data error occured!");
	}else{
		FileForSending.close();
		emit progressChanged(0);
		disconnect(socket, SIGNAL(bytesWritten(qint64)), this, SLOT(sendPartOfFile()));
		emit sendStatus("File is sent! Choose new file (and ip if necessary) to send one more! :)");
		fileAquired = false;
		socket->close();
		socket = nullptr;
	}
}
