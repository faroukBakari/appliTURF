#ifndef HISTORYWINDOW_H
#define HISTORYWINDOW_H

#include <QMainWindow>
#include <QtSql>
#include "imodel.h"
#include "idelegate.h"
#include "itableview.h"
#include "QProgressIndicator.h"

namespace Ui{class HistoryWindow;}

class HistoryWindow : public QMainWindow{
	Q_OBJECT

public:
	explicit HistoryWindow(iModel* mainviewmodel, iModel* summarymodel, QWidget *parent = 0);
	virtual ~HistoryWindow();

public slots:
	void setCurrentPos(int pos);
	void filterCheckBox();
	void FormatData(bool);

private:
	Ui::HistoryWindow *ui;
	iModel* mainViewModel;
	iModel* summaryModel;
	iModel* histModel = NULL;
	int currentPos = -1;
};

#endif // HISTORYWINDOW_H
