#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow){
	ui->setupUi(this);

	db = QSqlDatabase::addDatabase( "QMYSQL" );

	db.setHostName( "localhost" );
	db.setDatabaseName( "GENY_DATA" );

	db.setUserName( "farouk" );
	db.setPassword( "password" );

	if( !db.open() ){
		QMessageBox msgBox;
		msgBox.setText("DB CONNECT ERROR : " + db.lastError().text());
		msgBox.exec();
	}

	QSqlQuery dateRange(db.exec("SELECT min(DT), max(DT) FROM GENY_DATA.RACES"));
	dateRange.next();
	ui->dateEdit->setDateRange(dateRange.value(0).toDate(), dateRange.value(1).toDate());
	connect(ui->dateEdit,SIGNAL(dateChanged(QDate)),
			this, SLOT(dateChanged(QDate)));

	connect(ui->HyppoCombo,SIGNAL(currentIndexChanged(int)),
			this, SLOT(hyppoChanged(int)));

	ui->mainView->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ui->RaceCombo,SIGNAL(currentIndexChanged(int)),
			this, SLOT(refreshSummary(int)));

	ui->mainView->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ui->RaceCombo,SIGNAL(currentIndexChanged(int)),
			this, SLOT(refreshMainView(int)));

	ui->mainView->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ui->mainView,SIGNAL(customContextMenuRequested(QPoint)),
			this, SLOT(mainViewContextMenue(QPoint)));

	connect(ui->HistoryTrigger,SIGNAL(triggered(bool)),
			this, SLOT(HistoryTriggerSlot()));

	connect(ui->CompareTrigger,SIGNAL(triggered(bool)),
			this, SLOT(CompareTriggerSlot()));

	ui->dateEdit->setDate(dateRange.value(1).toDate());

    //ui->mainView->horizontalHeader()->setMovable(true);
	ui->mainView->horizontalHeader()->setDragEnabled(true);
	ui->mainView->horizontalHeader()->setDragDropMode(QAbstractItemView::InternalMove);

	ui->mainView->setEditTriggers(QAbstractItemView::AllEditTriggers);

	ui->mainView->setFixedColumnWidth("P", 25);

	ui->mainView->showGrid();

	ui->mainView->setAlternatingRowColors(true);

}

MainWindow::~MainWindow(){
	delete ui;
}

void MainWindow::dateChanged(const QDate& date){
	if(!HyppoModel){
		HyppoModel = new iModel(this);
		ui->HyppoCombo->setModel(HyppoModel);
		ui->HyppoCombo->setModelColumn(0);
	}
	QString query =
			QString("SELECT DISTINCT CONCAT(NB_GREP,'- ',HYPPO) as C2, HYPPO as C1 "
					"FROM GENY_DATA.RACES "
					"WHERE DT = '%1' ORDER BY C2").arg(date.toString(Qt::ISODate));
	qDebug() << query;
	HyppoModel->setQuery(query);
}

void MainWindow::hyppoChanged(int hyppo_idx){
	if(hyppo_idx < 0) return;
	if(!RaceModel){
		RaceModel = new iModel(this);
		ui->RaceCombo->setModel(RaceModel);
		ui->RaceCombo->setModelColumn(0);
	}
	QString query =
			QString("SELECT DISTINCT CONCAT(NB_RACE,'- ',R_NAME) as R_NAME, RACE_ID "
					"FROM GENY_DATA.RACES "
					"WHERE DT = '%1' and "
					"HYPPO = '%2' ORDER BY R_NAME").arg(ui->dateEdit->date().toString(Qt::ISODate), HyppoModel->SourceData(hyppo_idx, 1).toString());
	qDebug() << query;
	RaceModel->setQuery(query);
}

void MainWindow::refreshSummary(int raceComboIndex){
	if(raceComboIndex < 0) return;
	if(summaryModel == NULL){
		summaryModel = new iModel(this);
		ui->mainView->setModel(summaryModel);
	}

	QString query = QString("call GENY_DATA.RACE_SUMMARY(%1)").arg(
				RaceModel->data(RaceModel->index(raceComboIndex, 1)).toString());

	qDebug() << query;
	summaryModel->setQuery(query);

	ui->ERaceId->setText(summaryModel->data(0,"RACE_ID").toString());
	ui->EDepart->setText(summaryModel->data(0,"DEPART").toString());
	ui->EPiste->setText(summaryModel->data(0,"PIST").toString());
	ui->EPrix->setText(summaryModel->data(0,"PRIZE").toString());

	ui->EType->setText(summaryModel->data(0,"TYPE_C").toString());
	ui->EDiscipline->setText(summaryModel->data(0,"COURSE").toString());
	ui->ESpecialite->setText(summaryModel->data(0,"SPECIALITE").toString());
	ui->EDivision->setText(summaryModel->data(0,"categorie").toString());

	ui->EDistance->setText(summaryModel->data(0,"R_LENGHT").toString());
	ui->ECorde->setText(summaryModel->data(0,"CORDE").toString());
	ui->ETerrain->setText(summaryModel->data(0,"TERRAIN").toString());
	ui->ELien->setText(summaryModel->data(0,"RLINK").toString());


	ui->EEligibles->setText(summaryModel->data(0,"ELIGIBLES").toString());
	ui->EDetails->setText(summaryModel->data(0,"DETAILS_COURSE").toString());

}

void MainWindow::refreshMainView(int raceComboIndex){

	if(raceComboIndex < 0) return;
	if(mainViewModel == NULL){
		mainViewModel = new iModel(this);
		ui->mainView->setModel(mainViewModel);
		ui->mainView->itemDelegate()->addColStyle("COTE_30mn", "QProgressBar");
		ui->mainView->itemDelegate()->addColStyle("COTE_F", "QProgressBar");
		QList<QVariant> comboItems({"-", 1, 2, 3});
		ui->mainView->itemDelegate()->addColStyle("Play", "QComboBox", comboItems);
	}
	QString query =
			QString("call GENY_DATA.RACE_DATA(%1)").arg(
				RaceModel->data(RaceModel->index(raceComboIndex, 1)).toString());
	qDebug() << query;
	mainViewModel->setQuery(query);

	ui->mainView->setColumnHidden(mainViewModel->colIdx("HORSE_ID"), true);
	ui->mainView->setColumnHidden(mainViewModel->colIdx("COACH_ID"), true);
	ui->mainView->setColumnHidden(mainViewModel->colIdx("DRIVER_ID"), true);


	mainViewModel->setColumnData("RANK", Qt::AlignCenter, Qt::TextAlignmentRole);
	mainViewModel->setColumnData("RANK", QColor(211, 255, 14), Qt::BackgroundColorRole);
	mainViewModel->setColumnData("RANK", QFont("Ubuntu", 11, QFont::Bold), Qt::FontRole);

	ui->mainView->setColumnHidden(mainViewModel->colIdx("RANK"), true);
	ui->mainView->setColumnHidden(mainViewModel->colIdx("FAIL"), true);
	ui->mainView->setColumnHidden(mainViewModel->colIdx("UNIT_TIME"), true);
	ui->mainView->setColumnHidden(mainViewModel->colIdx("VISUAL_GAP"), true);

	//decorating coachs & drivers cells
	QMap<QString, int> coachs;
	QString coach;
	for(int row = 0; row < mainViewModel->rowCount(); row++){
		if(mainViewModel->SourceData(row, "DRIVER") == mainViewModel->SourceData(row, "COACH")){
			mainViewModel->setData(row, "COACH", QColor(153, 255, 255), Qt::BackgroundColorRole);
			mainViewModel->setData(row, "DRIVER", QColor(153, 255, 255), Qt::BackgroundColorRole);
		}
		coach = mainViewModel->SourceData(row, "COACH").toString();
		coachs[coach] = coachs.contains(coach) ? coachs[coach] + 1 : 1;
	}
	for(auto& c : coachs.keys()){
		if(coachs.value(c) > 1){
			for(int row = 0; row < mainViewModel->rowCount(); row++){
				if(mainViewModel->SourceData(row, "COACH") == c)
					mainViewModel->setData(row, "COACH", QColor(60, 0, 255), Qt::ForegroundRole);
			}
		}
	}

	mainViewModel->addNewColumn("Play");
	mainViewModel->setColumnData("Play", Qt::AlignCenter, Qt::TextAlignmentRole);
	mainViewModel->setColumnData("Play", QFont("Ubuntu", 11, QFont::Bold), Qt::FontRole);
	mainViewModel->setColumnFlag("Play", Qt::ItemIsEditable);

	QFont f("Ubuntu", 11, QFont::StrikeOutResolved);
	f.setStrikeOut(true);
	for(int row = 0; row < mainViewModel->rowCount(); row++){
		if(mainViewModel->data(row, "FAIL").toString() == "NP"){
			mainViewModel->setRowData(row, f, Qt::FontRole);
            mainViewModel->setRowData(row, Qt::red, Qt::ForegroundRole);
		}
	}


	ui->BShowHideRes->setChecked(false);


}

void MainWindow::mainViewContextMenue(const QPoint& pos){
	ui->RunnersMenue->popup(ui->mainView->viewport()->mapToGlobal(pos));
}

void MainWindow::on_BShowHideRes_clicked(bool checked){

	ui->mainView->setColumnHidden(mainViewModel->colIdx("RANK"), !checked);
	ui->mainView->setColumnHidden(mainViewModel->colIdx("FAIL"), !checked);
	ui->mainView->setColumnHidden(mainViewModel->colIdx("UNIT_TIME"), !checked);
	ui->mainView->setColumnHidden(mainViewModel->colIdx("VISUAL_GAP"), !checked);


	if(checked){
		mainViewModel->clearColumnFlag("Play", Qt::ItemIsEditable);
		for(int row = 0; row < mainViewModel->rowCount(); row++){
			int ans = mainViewModel->data(row, "Play").toInt();
			if(ans){
				if(	mainViewModel->data(row, "RANK").toInt() != ans){
					mainViewModel->setData(row, "Play", QColor(255, 65, 65), Qt::BackgroundRole);
					mainViewModel->setData(row, "Play", QColor(255, 255, 255), Qt::ForegroundRole);
					qDebug() << "Wrong answer : " << mainViewModel->data(row, "RANK").toInt()<< "->" << ans;
				}
				else{
					mainViewModel->setData(row, "Play", QColor(94, 150, 21), Qt::BackgroundRole);
					mainViewModel->setData(row, "Play", QColor(255, 255, 255), Qt::ForegroundRole);
					qDebug() << "Good answer : " << mainViewModel->data(row, "RANK").toInt()<< "->" << ans;
				}
			}
		}

	}
	else{
		mainViewModel->clearUsrColumnData("Play");
		mainViewModel->clearUsrColumnData("Play", Qt::BackgroundRole);
		mainViewModel->clearUsrColumnData("Play", Qt::ForegroundRole);
		mainViewModel->setColumnFlag("Play", Qt::ItemIsEditable);
	}



	//ui->mainView->refresh();
	ui->mainView->setColumnHidden(mainViewModel->colIdx("Play"), false);
}

void MainWindow::HistoryTriggerSlot(){
	qDebug() << "New HistoryWindow instance";
	HistoryWindow *hw = new HistoryWindow(mainViewModel, summaryModel);
	hw->setAttribute( Qt::WA_DeleteOnClose );
	hw->show();
	qDebug() << "MainWindow - setCurrentPos : " << ui->mainView->currentIndex().row();
	hw->setCurrentPos(ui->mainView->currentIndex().row());
}

void MainWindow::CompareTriggerSlot(){
	qDebug() << "New duelswindow instance";
	DuelsWindow *dw = new DuelsWindow(mainViewModel, summaryModel);
	dw->setAttribute( Qt::WA_DeleteOnClose );
	dw->show();
}









