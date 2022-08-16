#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QFileDialog>
#include <QRegularExpressionValidator>
#include <QApplication>

class Client : public QObject
{
	Q_OBJECT
public:
	explicit Client(QObject *parent = nullptr);
	void setFilename(const QString &newFilename);
	void setIP(const QString&);
	void ContinueSending();
	void sendNameOfFile(QFile&);
	void ErrorOccured(QString);

signals:
	void progressChanged(int);
	void sendStatus(QString);

public slots:
	void sendPartOfFile();
	void SendFile();

private:

	QFile FileForSending;
	bool fileAquired;
	QString IP;
	bool ipAquired;

	qint64 sendSize; // для прогресс-бара

	qint16 port;
	QTcpSocket* socket;
};

#endif // CLIENT_H
