#include "duelwideget.h"
#include "ui_duelwideget.h"

DuelWideget::DuelWideget(QWidget *parent, iModel* source, int rId) :
	QGroupBox(parent),
	ui(new Ui::DuelWideget),
	DuelModel(new iModel(this)),
	rId(rId){
	ui->setupUi(this);
	DuelModel->setSourceModel(source);
	DuelModel->setDynamicSortFilter(true);
	DuelModel->addFilter("RACE_ID", QString::number(rId));
	setTitle(DuelModel->data(0,"DT").toString() + " - " +
			 DuelModel->data(0,"HYPPO").toString());
	ui->RaceSummary->setadjustHeightToContent();
	ui->RunnerPerf->setadjustHeightToContent();
	ui->RaceSummary->setModel(DuelModel);
	ui->RaceSummary->hideRow(1);
	ui->RaceSummary->hideColumn("RACE_ID");
	ui->RaceSummary->hideColumn("DT");
	ui->RaceSummary->hideColumn("HYPPO");
	ui->RunnerPerf->setModel(DuelModel);
	ui->RunnerPerf->hideColumn("HORSE_ID");
	ui->RunnerPerf->hideColumn("DRIVER_ID");
	ui->RunnerPerf->hideColumn("COACH_ID");
	ui->RaceSummary->HideEmptyColumns();
	ui->RunnerPerf->HideEmptyColumns();
	int edge =  DuelModel->colIdx("P");
	for(int i = 0; i < DuelModel->columnCount(); i++){
		if(i < edge) ui->RunnerPerf->hideColumn(i);
		else ui->RaceSummary->hideColumn(i);
	}
    ui->RunnerPerf->itemDelegate()->addColStyle("VALEUR", "QProgressBar");
    ui->RunnerPerf->itemDelegate()->addColStyle("WEIGHT", "QProgressBar");



}

DuelWideget::~DuelWideget(){
	delete ui;
	if(DuelModel) delete DuelModel;
}
