#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
//	double u = 24.65;
//	QVariant v(u);
//	qDebug() << v.toInt();
//	exit(0);
	QApplication a(argc, argv);
	MainWindow w;
	w.show();

	return a.exec();
}
