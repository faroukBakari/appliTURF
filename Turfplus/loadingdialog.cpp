#include "loadingdialog.h"
#include "ui_loadingdialog.h"
#include<QMovie>

LoadingDialog::LoadingDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::LoadingDialog){
	ui->setupUi(this);
	setAttribute(Qt::WA_TranslucentBackground);
	setWindowFlags(Qt::FramelessWindowHint);
    QMovie* animation = new QMovie("/home/farouk/qt-projects/Turfplus/loading.gif");
	ui->LAnimation->setMovie(animation);
	animation->start();

}

LoadingDialog::~LoadingDialog(){
	delete ui;
}
