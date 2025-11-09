#ifndef DUELSWINDOW_H
#define DUELSWINDOW_H

#include <QMainWindow>
#include <QtSql>
#include "imodel.h"
#include "idelegate.h"
#include "itableview.h"
#include "QProgressIndicator.h"
#include "duelwideget.h"

namespace Ui {class duelswindow;}

class DuelsWindow : public QMainWindow{
	Q_OBJECT

public:
	explicit DuelsWindow(iModel* mainviewmodel, iModel* summarymodel, QWidget *parent = 0);
	virtual ~DuelsWindow();

public slots:
    void updateData(bool yes = true);
	void updateDuel(const QString& source);
	void prepareLayout(bool);

private:
    Ui::duelswindow *ui = NULL;
    iModel* mainViewModel = NULL;
    iModel* summaryModel = NULL;
    QSignalMapper* signalMapper = NULL;
	iModel* DuelModel = NULL;
	QMap<QString, QComboBox*> CBListA;
	QMap<QString, QComboBox*> CBListB;
	int posA = -1;
	int posB = -1;
	QMap<int, QString> horses, drivers, coaches;
};

#endif // DUELSWINDOW_H
