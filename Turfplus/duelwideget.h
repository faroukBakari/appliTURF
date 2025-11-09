#ifndef DUELWIDEGET_H
#define DUELWIDEGET_H

#include <QGroupBox>
#include "imodel.h"

namespace Ui {
class DuelWideget;
}

class DuelWideget : public QGroupBox
{
	Q_OBJECT

public:
	explicit DuelWideget(QWidget *parent, iModel*, int rId);
	~DuelWideget();

private:
	Ui::DuelWideget *ui;
	iModel* DuelModel = NULL;
	int rId = 0;
};

#endif // DUELWIDEGET_H
