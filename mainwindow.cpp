#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
{
	ui->setupUi(this);




	connect(ui->pushButton, &QPushButton::clicked, &serv, &Server::startServer);
	connect(&serv, &Server::sendStatus, ui->textBrowser, &QTextBrowser::append);
	connect(&client, &Client::sendStatus, ui->textBrowser, &QTextBrowser::append);
	connect(ui->pushButton_2, &QPushButton::clicked, &client, &Client::SendFile);
	connect(ui->pushButton_2, &QPushButton::clicked, &serv, &Server::stoppingListening);
	connect(&serv, &Server::progressChanged, ui->progressBar, &QProgressBar::setValue);
	connect(&client, &Client::progressChanged, ui->progressBar, &QProgressBar::setValue);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::on_pushButton_3_clicked()
{
	client.setFilename(QFileDialog::getOpenFileName(this, tr("Open File"),"/home"));
}


void MainWindow::on_lineEdit_editingFinished()
{
	ui->lineEdit->deselect();
	client.setIP(ui->lineEdit->text());
	ui->lineEdit->clear();
}

