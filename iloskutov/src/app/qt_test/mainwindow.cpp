#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTimer>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    timer(new QTimer)
{
    ui->setupUi(this);
    this->showFullScreen();
	this->setFocus();
	repaint();
//	resize(400,180);
//	move(10,10);
	
//	timer->setInterval(500);
//	connect(timer, SIGNAL(timeout()), this, SLOT(on_refresh()));
//	timer->start();
	QTimer::singleShot(100, this, SLOT(on_refresh()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_refresh()
{
	repaint();
}

void MainWindow::on_pushButton_clicked()
{
    ui->label->setText("<font color=\"red\">Red</font>");
	repaint();
}

void MainWindow::on_pushButton_2_clicked()
{
    ui->label->setText("<font color=\"green\">Green</font>");
	repaint();
}

void MainWindow::on_pushButton_3_clicked()
{
    ui->label->setText("<font color=\"blue\">Blue</font>");
	repaint();
}
