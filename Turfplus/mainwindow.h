#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSql>
#include <QMessageBox>
#include "imodel.h"
#include "idelegate.h"
#include "itableview.h"
#include "historywindow.h"
#include "duelswindow.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	virtual ~MainWindow();

private slots:
	void dateChanged(const QDate&);
	void hyppoChanged(int index);
	void mainViewContextMenue(const QPoint&);

	void refreshMainView(int);
	void refreshSummary(int);


	void on_BShowHideRes_clicked(bool checked);

	void HistoryTriggerSlot();
	void CompareTriggerSlot();

private:
	Ui::MainWindow *ui;
	QSqlDatabase db;
	iModel *mainViewModel = NULL;
	iModel *summaryModel = NULL;
	iModel *HyppoModel = NULL;
	iModel *RaceModel = NULL;

};

#endif // MAINWINDOW_H
