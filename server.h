#ifndef SERVER_H
#define SERVER_H

#define DATA_BLOCK_SIZE 1000

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHostInfo>
#include <QDebug>
#include <QNetworkInterface>
#include <QApplication>
#include "qdir.h"

class Server : public QTcpServer
{
	Q_OBJECT
public:
	Server();
	void receiveFile();
	void ErrorOccured(QString);
signals:
	void sendStatus(QString);
	void progressChanged(int);
public slots:
	void incomingConnection(qintptr socketDescriptor);
	void slotReadyRead();
	void slotReadyReadName();
	void startServer();
	void stoppingListening();

private:

	QTcpSocket *socket;
	QFile data;
	QDataStream *in;
	qint64 fileSize, receivedSize;
};

#endif // SERVER_H
