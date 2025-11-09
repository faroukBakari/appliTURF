#include "duelswindow.h"
#include "ui_duelswindow.h"
#include <QtCore>
#include <QtGui>

class Sleeper : public QThread{
public:
	static void usleep(unsigned long usecs){QThread::usleep(usecs);}
	static void msleep(unsigned long msecs){QThread::msleep(msecs);}
	static void sleep(unsigned long secs){QThread::sleep(secs);}
};

DuelsWindow::DuelsWindow(iModel* mainviewmodel, iModel* summarymodel, QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::duelswindow),
	mainViewModel(mainviewmodel),
	summaryModel(summarymodel){

	ui->setupUi(this);


	ui->RecapView->setModel(summaryModel);
	ui->RecapView->verticalHeader()->hide();
	ui->RecapView->HideEmptyColumns();

	ui->RecapView->verticalHeader()->setResizeMode(0, QHeaderView::Stretch);

	ui->RecapView->hideColumn("RLINK");

	ui->RecapView->setWordWrap(true);

	ui->CBChevalA->setModel(mainViewModel);
	ui->CBJockeyA->setModel(mainViewModel);
	ui->CBCoachA->setModel(mainViewModel);

	ui->CBChevalA->setModelColumn(mainViewModel->colIdx("HORSE"));
	ui->CBJockeyA->setModelColumn(mainViewModel->colIdx("DRIVER"));
	ui->CBCoachA->setModelColumn(mainViewModel->colIdx("COACH"));

	ui->CBChevalA->setCurrentIndex(-1);
	ui->CBJockeyA->setCurrentIndex(-1);
	ui->CBCoachA->setCurrentIndex(-1);

	ui->CBChevalB->setModel(mainViewModel);
	ui->CBJockeyB->setModel(mainViewModel);
	ui->CBCoachB->setModel(mainViewModel);

	ui->CBChevalB->setModelColumn(mainViewModel->colIdx("HORSE"));
	ui->CBJockeyB->setModelColumn(mainViewModel->colIdx("DRIVER"));
	ui->CBCoachB->setModelColumn(mainViewModel->colIdx("COACH"));

	ui->CBChevalB->setCurrentIndex(-1);
	ui->CBJockeyB->setCurrentIndex(-1);
	ui->CBCoachB->setCurrentIndex(-1);

	QSignalMapper *signalMapper = new QSignalMapper(this);

	CBListA["CBChevalA"] = ui->CBChevalA;
	CBListA["CBJockeyA"] = ui->CBJockeyA;
	CBListA["CBCoachA"] = ui->CBCoachA;
	CBListB["CBChevalB"] = ui->CBChevalB;
	CBListB["CBJockeyB"] = ui->CBJockeyB;
	CBListB["CBCoachB"] = ui->CBCoachB;

	for(auto& e : CBListA.toStdMap()){
		signalMapper->setMapping(e.second, e.first);
		connect(e.second, SIGNAL(currentIndexChanged(int)), signalMapper, SLOT(map()));
	}
	for(auto& e : CBListB.toStdMap()){
		signalMapper->setMapping(e.second, e.first);
		connect(e.second, SIGNAL(currentIndexChanged(int)), signalMapper, SLOT(map()));
	}

	connect(signalMapper, SIGNAL(mapped(QString)), this, SLOT(updateDuel(QString)));

	DuelModel = new iModel(this);

    DuelModel->setTargetWidget(this);

	ui->scrollAreaWidgetContents->setLayout(new QVBoxLayout(ui->DuelsView));

	connect(DuelModel, SIGNAL(dataAvalable(bool)), this, SLOT(setEnabled(bool)));

	connect(DuelModel, SIGNAL(dataAvalable(bool)), this, SLOT(prepareLayout(bool)));

    connect(mainViewModel, SIGNAL(dataAvalable(bool)), this, SLOT(updateData(bool)));

    updateData();

}


void DuelsWindow::updateData(bool yes){

    if(!yes) return;

    QString query = QString("CALL GENY_DATA.ALL_DUELS(%1)").arg(
                    summaryModel->data(0, "RACE_ID").toString());

    qDebug() << query;
    DuelModel->setQueryAsync(query);

}

void DuelsWindow::prepareLayout(bool avalable){

	if(avalable){


        QFont f("Ubuntu", 11, QFont::StrikeOutResolved);
        f.setStrikeOut(true);
        for(int row = 0; row < DuelModel->rowCount(); row++){

            if(DuelModel->data(row, "FAIL").toString() == "NP"){
                DuelModel->setRowData(row, f, Qt::FontRole);
                DuelModel->setRowData(row, Qt::red, Qt::ForegroundRole);
                continue;
            }

            if(DuelModel->data(row,"CORDE").toString() == summaryModel->data(0, "CORDE").toString()){
                DuelModel->setData(row, "CORDE", QColor(60, 0, 255), Qt::ForegroundRole);
            }
            //posA

            if(DuelModel->data(row,"DRIVER_ID").toString() == mainViewModel->data(posA, "DRIVER_ID").toString()){
                DuelModel->setData(row, "DRIVER", QColor(60, 0, 255), Qt::ForegroundRole);
            }

            if(DuelModel->data(row,"COACH_ID").toString() == mainViewModel->data(posA, "COACH_ID").toString()){
                DuelModel->setData(row, "COACH", QColor(60, 0, 255), Qt::ForegroundRole);
            }

            if(DuelModel->data(row,"OEIL").toString() == mainViewModel->data(posA, "OEIL").toString()){
                DuelModel->setData(row, "OEIL", QColor(60, 0, 255), Qt::ForegroundRole);
            }

            if(DuelModel->data(row,"IRON").toString() == mainViewModel->data(posA, "IRON").toString()){
                DuelModel->setData(row, "IRON", QColor(60, 0, 255), Qt::ForegroundRole);
            }
            //posB


            if(DuelModel->data(row,"DRIVER_ID").toString() == mainViewModel->data(posB, "DRIVER_ID").toString()){
                DuelModel->setData(row, "DRIVER", QColor(60, 0, 255), Qt::ForegroundRole);
            }

            if(DuelModel->data(row,"COACH_ID").toString() == mainViewModel->data(posB, "COACH_ID").toString()){
                DuelModel->setData(row, "COACH", QColor(60, 0, 255), Qt::ForegroundRole);
            }

            if(DuelModel->data(row,"OEIL").toString() == mainViewModel->data(posB, "OEIL").toString()){
                DuelModel->setData(row, "OEIL", QColor(60, 0, 255), Qt::ForegroundRole);
            }

            if(DuelModel->data(row,"IRON").toString() == mainViewModel->data(posB, "IRON").toString()){
                DuelModel->setData(row, "IRON", QColor(60, 0, 255), Qt::ForegroundRole);
            }

            if(DuelModel->data(row,"SPECIALITE").toString() == summaryModel->data(0, "SPECIALITE").toString()){
                DuelModel->setData(row, "SPECIALITE", QColor(60, 0, 255), Qt::ForegroundRole);
            }

            if(DuelModel->data(row,"COURSE").toString() == summaryModel->data(0, "COURSE").toString()){
                DuelModel->setData(row, "COURSE", QColor(60, 0, 255), Qt::ForegroundRole);
            }

            if(DuelModel->data(row,"HYPPO").toString() == (summaryModel->data(0,"HYPPO").toString() + QString(" ") + summaryModel->data(0,"PIST").toString())){
                DuelModel->setData(row, "HYPPO", QColor(60, 0, 255), Qt::ForegroundRole);
            }

            if(DuelModel->data(row,"TERRAIN").toString() == summaryModel->data(0,"TERRAIN").toString()){
                DuelModel->setData(row, "TERRAIN", QColor(60, 0, 255), Qt::ForegroundRole);
            }

            if(DuelModel->data(row,"TERRAIN").toString() == summaryModel->data(0,"TERRAIN").toString()){
                DuelModel->setData(row, "TERRAIN", QColor(60, 0, 255), Qt::ForegroundRole);
            }

            if(DuelModel->data(row,"LENGHT").toString() == summaryModel->data(0,"R_LENGHT").toString()){
                DuelModel->setData(row, "LENGHT", QColor(60, 0, 255), Qt::ForegroundRole);
            }

            int r = DuelModel->data(row,"RANK").toInt();
            if(r){
                if(r == 1 ){
                    DuelModel->setData(row, "RANK", QColor(145, 255, 0), Qt::BackgroundColorRole);
                }
                else if(r <= 3 ){
                    DuelModel->setData(row, "RANK", QColor(153, 255, 255), Qt::BackgroundColorRole);
                }
            }
        }


		qDebug() << "Preparing layout...";
		QMap<int, int> races;
		for(int row = 0; row < DuelModel->rowCount(); row++){
			if(races.contains(DuelModel->data(row, "RACE_ID").toInt()))
				races[DuelModel->data(row, "RACE_ID").toInt()] += 1;
			else
				races[DuelModel->data(row, "RACE_ID").toInt()] = 1;
		}
		for (auto race : races.keys()){
			if(races[race] < 2) {
				qDebug() << "Race ignored :" << race << "--> nb = " << races[race];
				continue;
			}

			qDebug() << "Race :" << race;
			auto Dw = new DuelWideget(this, DuelModel, race);
			ui->scrollAreaWidgetContents->layout()->addWidget(Dw);
		}
	}
	else
		qDeleteAll(ui->scrollAreaWidgetContents->findChildren<DuelWideget*>());

}

void DuelsWindow::updateDuel(const QString& source){

	if(CBListA.contains(source)){

		if(CBListA[source]->currentIndex() == posA) return;

		prepareLayout(false);

		DuelModel->removeColumnFilters("HORSE_ID");

		posA = CBListA[source]->currentIndex();
		for(auto e : CBListA.values())
			if(e->currentIndex() != posA) e->setCurrentIndex(posA);

	}
	else{

		if(CBListB[source]->currentIndex() == posB) return;

		prepareLayout(false);

		DuelModel->removeColumnFilters("HORSE_ID");

		posB = CBListB[source]->currentIndex();
		for(auto e : CBListB.values())
			if(e->currentIndex() != posB) e->setCurrentIndex(posB);


	}

	DuelModel->addFilter("HORSE_ID", mainViewModel->data(posA, "HORSE_ID").toString());
	DuelModel->addFilter("HORSE_ID", mainViewModel->data(posB, "HORSE_ID").toString());
	prepareLayout(true);

}

DuelsWindow::~DuelsWindow(){
	delete ui;

	if(DuelModel) delete DuelModel;

}







































