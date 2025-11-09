#include "historywindow.h"
#include "ui_historywindow.h"

HistoryWindow::HistoryWindow(iModel* mainviewmodel, iModel* summarymodel, QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::HistoryWindow),
	mainViewModel(mainviewmodel),
	summaryModel(summarymodel){
	ui->setupUi(this);

	ui->CBCheval->setModel(mainViewModel);
	ui->CBJokey->setModel(mainViewModel);
	ui->CBTrainer->setModel(mainViewModel);

	ui->CBCheval->setModelColumn(mainViewModel->colIdx("HORSE"));
	ui->CBJokey->setModelColumn(mainViewModel->colIdx("DRIVER"));
	ui->CBTrainer->setModelColumn(mainViewModel->colIdx("COACH"));

	ui->CBCheval->setCurrentIndex(-1);
	ui->CBJokey->setCurrentIndex(-1);
	ui->CBTrainer->setCurrentIndex(-1);

	connect(ui->CBCheval, SIGNAL(currentIndexChanged(int)),
			this, SLOT(setCurrentPos(int)));
	connect(ui->CBJokey, SIGNAL(currentIndexChanged(int)),
			this, SLOT(setCurrentPos(int)));
	connect(ui->CBTrainer, SIGNAL(currentIndexChanged(int)),
			this, SLOT(setCurrentPos(int)));


	//Filtering
	connect(ui->ChBCheval, SIGNAL(toggled(bool)), this, SLOT(filterCheckBox()));
	connect(ui->ChBJockey, SIGNAL(toggled(bool)), this, SLOT(filterCheckBox()));
	connect(ui->ChBTrainer, SIGNAL(toggled(bool)), this, SLOT(filterCheckBox()));

	histModel = new iModel(this);
	ui->HistView->setModel(histModel);
	histModel->setDynamicSortFilter(true);
	connect(histModel, SIGNAL(dataAvalable(bool)), this, SLOT(setEnabled(bool)));
	connect(histModel, SIGNAL(dataAvalable(bool)), this, SLOT(FormatData(bool)));

	ui->HistView->horizontalHeader()->setMovable(true);
	ui->HistView->horizontalHeader()->setDragEnabled(true);
	ui->HistView->horizontalHeader()->setDragDropMode(QAbstractItemView::InternalMove);


	ui->HistView->setFixedColumnWidth("P", 25);
	ui->HistView->setFixedColumnWidth("DT", 90);

	ui->HistView->showGrid();

}

HistoryWindow::~HistoryWindow(){
	delete ui;
	if(histModel) delete histModel;
}

void HistoryWindow::filterCheckBox(){

	if(ui->ChBCheval->checkState() != 0){
		qDebug() << "Filtering Horse :" << ui->CBCheval->currentText();
		histModel->addFilter("HORSE_ID", mainViewModel->data(ui->CBCheval->currentIndex(), "HORSE_ID").toString());
		if(histModel->rowCount()) ui->HistView->hideColumn("HORSE");
	}
	else{
		qDebug() << "Reseting Horse Filter";
		histModel->removeFilter("HORSE_ID", mainViewModel->data(ui->CBCheval->currentIndex(), "HORSE_ID").toString());
		if(histModel->rowCount()) ui->HistView->showColumn("HORSE");
	}

	if(ui->ChBJockey->checkState() != 0){
		qDebug() << "Filtering Horse :" << ui->CBJokey->currentText();
		histModel->addFilter("DRIVER_ID", mainViewModel->data(ui->CBJokey->currentIndex(), "DRIVER_ID").toString());
		if(histModel->rowCount()) ui->HistView->hideColumn("DRIVER");
	}
	else{
		qDebug() << "Reseting driver Filter";
		histModel->removeFilter("DRIVER_ID", mainViewModel->data(ui->CBJokey->currentIndex(), "DRIVER_ID").toString());
		if(histModel->rowCount()) ui->HistView->showColumn("DRIVER");
	}

	if(ui->ChBTrainer->checkState() != 0){
		qDebug() << "Filtering Horse :" << ui->CBTrainer->currentText();
		histModel->addFilter("COACH_ID", mainViewModel->data(ui->CBTrainer->currentIndex(), "COACH_ID").toString());
		if(histModel->rowCount()) ui->HistView->hideColumn("COACH");
	}
	else{
		qDebug() << "Reseting coach Filter";
		histModel->removeFilter("COACH_ID", mainViewModel->data(ui->CBTrainer->currentIndex(), "COACH_ID").toString());
		if(histModel->rowCount()) ui->HistView->showColumn("COACH");
	}
}

void HistoryWindow::setCurrentPos(int pos){

	qDebug() << "setCurrentPos : " << currentPos << "-->" << pos;

	if(currentPos == pos) return;

	currentPos = pos;

	if(ui->CBCheval->currentIndex() != currentPos) ui->CBCheval->setCurrentIndex(currentPos);
	if(ui->CBJokey->currentIndex() != currentPos) ui->CBJokey->setCurrentIndex(currentPos);
	if(ui->CBTrainer->currentIndex() != currentPos) ui->CBTrainer->setCurrentIndex(currentPos);

	ui->LCasaque->setPixmap(mainViewModel->data(pos, "CASAQ", Qt::DecorationRole).value<QPixmap>());

	/*
	 * mainViewModel columns :
	 * =======================
	 * rdata.HORSE_ID,
	 * casaq.IMG as CASAQ,
	 * rdata.POS_DEPART as P,
	 * rdata.HORSE,
	 * DRIVER,
	 * COACH,
	 * oei.DESCR as OEIL,
	 * HR_LENGTH,
	 * iron.DESCR as IRON_REMOV,
	 * DECHARGE,
	 * CONCAT(SEX, AGE) as SEX_AGE,
	 * WEIGHT,
	 * DRIVER_ID,
	 * COACH_ID,
	 * MUSIC,
	 * WINNING,
	 * VALEUR,
	 * cotes.COTE_30mn,
	 * pic.IMG as EVOL,
	 * cotes.COTE_F,
	 * rslt.UNIT_TIME,
	 * rslt.VISUAL_GAP,
	 * rslt.FAIL,
	 * rslt.RANK
	*/

	QVariant tmp;
	tmp = mainViewModel->data(pos, "SEX_AGE"); ui->ESexAge->setText(tmp.isNull() ? "" : tmp.toString());
	tmp = mainViewModel->data(pos, "VALEUR"); ui->EValeur->setText(tmp.isNull() ? "" : tmp.toString());
	tmp = mainViewModel->data(pos, "WINNING"); ui->ECumulGain->setText(tmp.isNull() ? "" : tmp.toString());

	tmp = mainViewModel->data(pos, "OEIL"); ui->EOeuil->setText(tmp.isNull() ? "" : tmp.toString());
	tmp = mainViewModel->data(pos, "IRON_REMOV"); ui->EDeferrage->setText(tmp.isNull() ? "" : tmp.toString());
	tmp = summaryModel->data(0, "CORDE"); ui->ECorde->setText(tmp.isNull() ? "" : tmp.toString());

	tmp = mainViewModel->data(pos, "COTE_30mn"); ui->ECoteInit->setText(tmp.isNull() ? "" : tmp.toString());
	ui->ECoteEvol->setPixmap(mainViewModel->data(pos, "EVOL", Qt::DecorationRole).value<QPixmap>());
	tmp = mainViewModel->data(pos, "COTE_F"); ui->ECoteF->setText(tmp.isNull() ? "" : tmp.toString());

	if(currentPos == -1) {
		setEnabled(false);
		return;
	}

	QString query = QString("CALL GENY_DATA.MULTI_HIST(%1,%2,%3,'%4')").arg(
					mainViewModel->data(pos, "HORSE_ID").toString(),
					mainViewModel->data(pos, "DRIVER_ID").toString(),
					mainViewModel->data(pos, "COACH_ID").toString(),
					summaryModel->data(0, "DATE").toString());
	qDebug() << query;
	histModel->setQueryAsync(query);

}

void HistoryWindow::FormatData(bool avalable){

	if(!avalable) return;

	ui->HistView->hideColumn(histModel->colIdx("HORSE_ID"));
	ui->HistView->hideColumn(histModel->colIdx("DRIVER_ID"));
	ui->HistView->hideColumn(histModel->colIdx("COACH_ID"));

	QFont f("Ubuntu", 11, QFont::StrikeOutResolved);
	f.setStrikeOut(true);
	for(int row = 0; row < histModel->rowCount(); row++){

		if(histModel->data(row, "FAIL").toString() == "NP"){
			histModel->setRowData(row, f, Qt::FontRole);
			histModel->setRowData(row, Qt::red, Qt::ForegroundRole);
			continue;
		}

		if(histModel->data(row,"CORDE").toString() == summaryModel->data(0, "CORDE").toString()){
			histModel->setData(row, "CORDE", QColor(60, 0, 255), Qt::ForegroundRole);
		}

		if(histModel->data(row,"HORSE_ID").toString() == mainViewModel->data(currentPos, "HORSE_ID").toString()){
			histModel->setData(row, "HORSE", QColor(60, 0, 255), Qt::ForegroundRole);
		}

		if(histModel->data(row,"DRIVER_ID").toString() == mainViewModel->data(currentPos, "DRIVER_ID").toString()){
			histModel->setData(row, "DRIVER", QColor(60, 0, 255), Qt::ForegroundRole);
		}

		if(histModel->data(row,"COACH_ID").toString() == mainViewModel->data(currentPos, "COACH_ID").toString()){
			histModel->setData(row, "COACH", QColor(60, 0, 255), Qt::ForegroundRole);
		}

		if(histModel->data(row,"OEIL").toString() == mainViewModel->data(currentPos, "OEIL").toString()){
			histModel->setData(row, "OEIL", QColor(60, 0, 255), Qt::ForegroundRole);
		}

		if(histModel->data(row,"IRON").toString() == mainViewModel->data(currentPos, "IRON").toString()){
			histModel->setData(row, "IRON", QColor(60, 0, 255), Qt::ForegroundRole);
		}

		if(histModel->data(row,"SPECIALITE").toString() == summaryModel->data(0, "SPECIALITE").toString()){
			histModel->setData(row, "SPECIALITE", QColor(60, 0, 255), Qt::ForegroundRole);
		}

		if(histModel->data(row,"COURSE").toString() == summaryModel->data(0, "COURSE").toString()){
			histModel->setData(row, "COURSE", QColor(60, 0, 255), Qt::ForegroundRole);
		}

		if(histModel->data(row,"HYPPO").toString() == (summaryModel->data(0,"HYPPO").toString() + QString(" ") + summaryModel->data(0,"PIST").toString())){
			histModel->setData(row, "HYPPO", QColor(60, 0, 255), Qt::ForegroundRole);
		}

		if(histModel->data(row,"TERRAIN").toString() == summaryModel->data(0,"TERRAIN").toString()){
			histModel->setData(row, "TERRAIN", QColor(60, 0, 255), Qt::ForegroundRole);
		}

		if(histModel->data(row,"TERRAIN").toString() == summaryModel->data(0,"TERRAIN").toString()){
			histModel->setData(row, "TERRAIN", QColor(60, 0, 255), Qt::ForegroundRole);
		}

		if(histModel->data(row,"LENGHT").toString() == summaryModel->data(0,"R_LENGHT").toString()){
			histModel->setData(row, "LENGHT", QColor(60, 0, 255), Qt::ForegroundRole);
		}

		int r = histModel->data(row,"RANK").toInt();
		if(r){
			if(r == 1 ){
				histModel->setData(row, "RANK", QColor(145, 255, 0), Qt::BackgroundColorRole);
			}
			else if(r <= 3 ){
				histModel->setData(row, "RANK", QColor(153, 255, 255), Qt::BackgroundColorRole);
			}
		}

		ui->HistView->itemDelegate()->addColStyle("VALEUR", "QProgressBar");
		ui->HistView->itemDelegate()->addColStyle("WEIGHT", "QProgressBar");

	}

	filterCheckBox();

}

























