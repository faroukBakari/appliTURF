#include "itableview.h"
#include <QDebug>
#include <mainwindow.h>
#include <QtGui>



class FlipModel : public iModel {
public:
	FlipModel(QObject * parent = 0);
	QModelIndex mapToSource(const QModelIndex &proxyIndex) const;
	QModelIndex mapFromSource(const QModelIndex &sourceIndex) const;
	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex &child) const;
	int rowCount(const QModelIndex &parent) const;
	int columnCount(const QModelIndex &parent) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const;
};

FlipModel::FlipModel(QObject *parent) :
	iModel(parent) {
}

QModelIndex FlipModel::mapToSource(const QModelIndex &proxyIndex) const {
	if (sourceModel()) {
		return sourceModel()->index(proxyIndex.column(), proxyIndex.row());
	} else {
		return QModelIndex();
	}
}

QModelIndex FlipModel::mapFromSource(const QModelIndex &sourceIndex) const {
	return index(sourceIndex.column(), sourceIndex.row());
}

QModelIndex FlipModel::index(int row, int column, const QModelIndex &) const {
	return createIndex(row, column, (void*) 0);
}

QModelIndex FlipModel::parent(const QModelIndex &) const {
	return QModelIndex();
}

int FlipModel::rowCount(const QModelIndex &) const {
	return sourceModel() ? sourceModel()->columnCount() : 0;
}

int FlipModel::columnCount(const QModelIndex &) const {
	return sourceModel() ? sourceModel()->rowCount() : 0;
}

QVariant FlipModel::headerData(
		int section, Qt::Orientation orientation, int role) const {
	if (!sourceModel()) { return QVariant(); }
	Qt::Orientation new_orientation = orientation == Qt::Horizontal ?
				Qt::Vertical : Qt::Horizontal;
	return sourceModel()->headerData(section, new_orientation, role);
}


void iTableView::setdebugFlag(bool flag){
	debugFlag = flag;
	qDebug()<<Q_FUNC_INFO;
	qDebug()<< "debugFlag = " << debugFlag;
}

void iTableView::setHideEmptyColumns(bool flag){
	hideEmptyColumns = flag;
	qDebug()<<Q_FUNC_INFO;
	qDebug()<< "hideEmptyColumns = " << hideEmptyColumns;
}

void iTableView::setFixedColumnWidth(const QString& col, int width){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	FixedColumnWidth.insert(col,width);
}
void iTableView::resetFixedColumnWidth(){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	FixedColumnWidth.clear();
}

void iTableView::PopAllEditors(){
	//reset();
	for(int row = 0; row < model()->rowCount(); row++)
		for(int col = 0; col < model()->columnCount(); col++){
			if(model()->index(row, col).flags() & Qt::ItemIsEditable)
				openPersistentEditor(model()->index(row, col));
			else
				closePersistentEditor(model()->index(row, col));
		}
}

void iTableView::HideEmptyColumns(){
	//hiding empty columns
	if(hideEmptyColumns){
		for(int col : hiddenCols)
			QTableView::setColumnHidden(col, false);

		hiddenCols.clear();

		for(int col = 0; col < model()->columnCount(); col++){
			if(isColumnHidden(col)) continue;
			int row = 0;
			for(; row < model()->rowCount(); row++)
				if (!model()->data(row,col).isNull() || model()->index(row,col).flags() & Qt::ItemIsEditable) break;

			if(row == model()->rowCount()){
				if(true) qDebug() << "Hiding empty column " << model()->headerData(col, Qt::Horizontal).toString();
				QTableView::setColumnHidden(col, true);
				hiddenCols.insert(col);
			}
		}

		ResizeColumns(this->width());
	}
}

void iTableView::ResizeColumns(int parentWidth){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;

	if(!model()) return;

	int     vis_cols = 0,
			nb_col = model()->columnCount(),
			winwidth = parentWidth - 2 * verticalHeader()->width();

	if(winwidth < 500) return;

	QString colName;
	for(int col = 0; col < nb_col; col++){
		if(!isColumnHidden(col)){
			colName = model()->headerData(col, Qt::Horizontal).toString();
			if(FixedColumnWidth.contains(colName)){
				setColumnWidth(col, FixedColumnWidth.value(colName));
				winwidth -= FixedColumnWidth.value(colName);
			}
			else vis_cols++;
		}
	}

	int lastVisibleCol = -1;
	for(int col = 0; col < nb_col; col++){
		if(!isColumnHidden(col)){
			lastVisibleCol = col;
			colName = model()->headerData(col, Qt::Horizontal).toString();
			if(!FixedColumnWidth.contains(colName)){
				setColumnWidth(col, winwidth/vis_cols);
			}

		}
	}

	if(lastVisibleCol > 0) horizontalHeader()->setResizeMode(lastVisibleCol, QHeaderView::Stretch);
}

void iTableView::AdjustHeightToContent(){
	int rowTotalHeight=0;

	// Rows height
	int count=verticalHeader()->count();
	for (int i = 0; i < count; ++i) {
		// 2018-03 edit: only account for row if it is visible
		if (!verticalHeader()->isSectionHidden(i)) {
			rowTotalHeight+=verticalHeader()->sectionSize(i);
		}
	}

	// Check for scrollbar visibility
//	if (!horizontalScrollBar()->isHidden())
//	{
//		 rowTotalHeight+=horizontalScrollBar()->height();
//	}

	// Check for header visibility
	if (!horizontalHeader()->isHidden())
	{
		 rowTotalHeight+=horizontalHeader()->height();
	}

	setFixedHeight(rowTotalHeight);
}

void iTableView::hideColumn(const QString& column){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	QTableView::hideColumn(model()->colIdx(column));
}

void iTableView::showColumn(const QString& column){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	QTableView::showColumn(model()->colIdx(column));
}

void iTableView::resetFilterMenus(){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;

	QMap<QString, QMenu*>& toFree = columnFilters;
	columnFilters = QMap<QString, QMenu*>();
	for(auto menue : toFree.values()){
		for(auto action : menue->actions())
			free(action);
		free(menue);
	}
	toFree.clear();

	for(int col = 0; col < model()->columnCount(); col++){
		QString colName = model()->headerData(col, Qt::Horizontal).toString();
		QMenu *ctxMenue = new QMenu(colName, this);
		connect(ctxMenue, SIGNAL(triggered(QAction*)), model(), SLOT(filterSlot(QAction*)));
		connect(ctxMenue, SIGNAL(triggered(QAction*)), this, SLOT(repopHeaderMenu(QAction*)));
		ctxMenue->setStyleSheet ("* { menu-scrollable: 1 }") ;

		QSet<QString> values;
		for(int row = 0; row < model()->rowCount(); row++)
			values.insert(model()->data(row,col).toString());

		QAction *action = new QAction("Clear_All", ctxMenue);
		ctxMenue->addAction(action);
		ctxMenue->addSeparator();
		ctxMenue->addSeparator();
		ctxMenue->addSeparator();
		ctxMenue->addSeparator();

		for(auto& str : values){
			action = new QAction(str, ctxMenue);
			action->setCheckable(true);
			action->setChecked(model()->isFiltred(colName, str));
			ctxMenue->addAction(action);
			horizontalHeader()->addAction(action);
		}

		columnFilters[colName] = ctxMenue;

	}
}

void iTableView::contextFilterMenu(const QPoint &point){
	if(true) qDebug()<<Q_FUNC_INFO;

	int col = horizontalHeader()->logicalIndexAt(point.x());
	qDebug() << "Hcolumn =" << col;
	QString colName = model()->headerData(col, Qt::Horizontal).toString();
	QMenu * ctxMenue = columnFilters.value(colName);

//	if(!ctxMenue) {
//		resetFilterMenus();
//		ctxMenue = columnFilters.value(colName);
//	}

	QSet<QString> values;
	for(int row = 0; row < model()->rowCount(); row++)
		values.insert(model()->data(row,col).toString());

	bool filtred = false;
	for(auto act : ctxMenue->actions()){
		act->setChecked(model()->isFiltred(colName, act->text()));
		if(model()->isFiltred(ctxMenue->title(), act->text())) filtred = true;
		act->setDisabled(act->text() != "Clear_All" && !values.contains(act->text()));
	}
	QFont font = qvariant_cast<QFont>(model()->headerData(ctxMenue->title(), Qt::Horizontal, Qt::FontRole));
	font.setBold(filtred);
	model()->setHeaderData(ctxMenue->title(), Qt::Horizontal, font, Qt::FontRole);
	if(filtred) horizontalHeader()->setFilterArrow(model()->colIdx(ctxMenue->title()));
	else  horizontalHeader()->removeFilterArrow(model()->colIdx(ctxMenue->title()));
	ctxMenue->popup(horizontalHeader()->viewport()->mapToGlobal(point));

}

void iTableView::repopHeaderMenu(QAction* act){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	QMenu * ctxMenue = (QMenu *)act->parent();

	bool filtred = false;
	for(auto act : ctxMenue->actions()){
		act->setChecked(model()->isFiltred(ctxMenue->title(), act->text()));
		if(model()->isFiltred(ctxMenue->title(), act->text())) filtred = true;
	}
	QFont font = qvariant_cast<QFont>(model()->headerData(ctxMenue->title(), Qt::Horizontal, Qt::FontRole));
	font.setBold(filtred);
	model()->setHeaderData(ctxMenue->title(), Qt::Horizontal, font, Qt::FontRole);
	if(filtred) horizontalHeader()->setFilterArrow(model()->colIdx(ctxMenue->title()));
	else  horizontalHeader()->removeFilterArrow(model()->colIdx(ctxMenue->title()));
	ctxMenue->show();

}

void iTableView::setFlipTable(bool flipFlag){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	flipTable = flipFlag;
	if(source) setModel(source);
}


void iTableView::setadjustHeightToContent(bool active){
	adjustHeightToContent = active;
	if(adjustHeightToContent) AdjustHeightToContent();
}

/*****************************************************************/

iDelegate *iTableView::itemDelegate() const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return (iDelegate*) QTableView::itemDelegate();
}

iDelegate *iTableView::horizontalHeaderItemDelegate() const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return (iDelegate*) horizontalHeader()->itemDelegate();
}

iDelegate *iTableView::verticalHeaderItemDelegate() const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return (iDelegate*) verticalHeader()->itemDelegate();
}

iTableView::iTableView(QWidget *parent) : QTableView(parent) {
	setHorizontalHeader(new iHeaderView(Qt::Horizontal, this));
	setVerticalHeader(new iHeaderView(Qt::Vertical, this));
	flip = new FlipModel(this);
	QTableView::setItemDelegate(new iDelegate(this));
	horizontalHeader()->setHighlightSections(false);
	horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(horizontalHeader(), SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextFilterMenu(QPoint)));
}

iTableView::~iTableView() {
	if(flip) delete flip;
}


void iTableView::setModel(iModel *newModel){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	if(model()) {
		disconnect(model(),SIGNAL(layoutChanged()), this, SLOT(HideEmptyColumns()));
		disconnect(model(),SIGNAL(layoutChanged()), this, SLOT(PopAllEditors()));
		disconnect(model(),SIGNAL(dataAvalable(bool)), this, SLOT(resetFilterMenus()));
	}

	flip->setSourceModel(newModel);
	source = newModel;
	QTableView::setModel(flipTable ? (iModel*)flip : (iModel*)source);

	connect(newModel,SIGNAL(layoutChanged()), this, SLOT(HideEmptyColumns()));
	connect(newModel,SIGNAL(layoutChanged()), this, SLOT(PopAllEditors()));
	connect(newModel,SIGNAL(dataAvalable(bool)), this, SLOT(resetFilterMenus()));
	newModel->setTargetWidget(this);

	HideEmptyColumns();
	PopAllEditors();
	resetFilterMenus();

}

iModel* iTableView::model() const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return (iModel*)QTableView::model();
}

void iTableView::setRootIndex(const QModelIndex &index){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	QTableView::setRootIndex(index);
}
void iTableView::setSelectionModel(QItemSelectionModel *selectionModel){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	QTableView::setSelectionModel(selectionModel);
}
void iTableView::doItemsLayout(){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	QTableView::doItemsLayout();
}

iHeaderView *iTableView::horizontalHeader() const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return (iHeaderView*)(QTableView::horizontalHeader());
}
iHeaderView *iTableView::verticalHeader() const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return (iHeaderView*)QTableView::verticalHeader();
}
void iTableView::setHorizontalHeader(iHeaderView *header){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	QTableView::setHorizontalHeader((QHeaderView*)header);
}
void iTableView::setVerticalHeader(iHeaderView *header){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	QTableView::setVerticalHeader((QHeaderView*)header);
}




int iTableView::rowViewportPosition(int row) const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return QTableView::rowViewportPosition(row);
}
int iTableView::rowAt(int y) const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return QTableView::rowAt(y);
}

void iTableView::setRowHeight(int row, int height){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	QTableView::setRowHeight(row, height);
}
int iTableView::rowHeight(int row) const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return QTableView::rowHeight(row);
}

int iTableView::columnViewportPosition(int column) const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return QTableView::columnViewportPosition(column);
}
int iTableView::columnAt(int x) const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return QTableView::columnAt(x);
}
void iTableView::setColumnWidth(int column, int width){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	QTableView::setColumnWidth(column, width);
}
int iTableView::columnWidth(int column) const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return QTableView::columnWidth(column);
}

bool iTableView::isRowHidden(int row) const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return QTableView::isRowHidden(row);
}
void iTableView::setRowHidden(int row, bool hide){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	QTableView::setRowHidden(row, hide);
	if(adjustHeightToContent) AdjustHeightToContent();
}

bool iTableView::isColumnHidden(int column) const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return QTableView::isColumnHidden(column);
}
void iTableView::setColumnHidden(int column, bool hide){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	QTableView::setColumnHidden(column, hide);
	ResizeColumns(this->width());
}

void iTableView::setSortingEnabled(bool enable){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	QTableView::setSortingEnabled(enable);
}
bool iTableView::isSortingEnabled() const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return QTableView::isSortingEnabled();
}

bool iTableView::showGrid() const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return QTableView::showGrid();
}


Qt::PenStyle iTableView::gridStyle() const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return QTableView::gridStyle();
}
void iTableView::setGridStyle(Qt::PenStyle style){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return QTableView::setGridStyle(style);
}

void iTableView::setWordWrap(bool on){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return QTableView::setWordWrap(on);
}
bool iTableView::wordWrap() const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return QTableView::wordWrap();
}

void iTableView::setCornerButtonEnabled(bool enable){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return QTableView::setCornerButtonEnabled(enable);
}
bool iTableView::isCornerButtonEnabled() const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return QTableView::isCornerButtonEnabled();
}

QRect iTableView::visualRect(const QModelIndex &index) const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return QTableView::visualRect(index);
}
void iTableView::scrollTo(const QModelIndex &index, ScrollHint hint){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return QTableView::scrollTo(index, hint);
}
QModelIndex iTableView::indexAt(const QPoint &p) const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return QTableView::indexAt(p);
}

void iTableView::setSpan(int row, int column, int rowSpan, int columnSpan){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return QTableView::setSpan(row, column, rowSpan, columnSpan);
}
int iTableView::rowSpan(int row, int column) const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return QTableView::rowSpan(row,column);
}
int iTableView::columnSpan(int row, int column) const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return QTableView::columnSpan(row,column);
}
void iTableView::clearSpans(){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return QTableView::clearSpans();
}

void iTableView::sortByColumn(int column, Qt::SortOrder order){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return QTableView::sortByColumn(column, order);
}


void iTableView::selectRow(int row){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	QTableView::selectRow(row);
}
void iTableView::selectColumn(int column){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	QTableView::selectColumn(column);
}
void iTableView::hideRow(int row){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	QTableView::hideRow(row);
}
void iTableView::hideColumn(int column){
	if(debugFlag) {
		qDebug()<<Q_FUNC_INFO;
		qDebug()<< "iTableView::hideColumn" << column;
	}
	QTableView::hideColumn(column);
}
void iTableView::showRow(int row){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	QTableView::showRow(row);
}
void iTableView::showColumn(int column){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	QTableView::showColumn(column);
}
void iTableView::resizeRowToContents(int row){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	QTableView::resizeRowToContents(row);
}
void iTableView::resizeRowsToContents(){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	QTableView::resizeRowsToContents();
}
void iTableView::resizeColumnToContents(int column){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	QTableView::resizeColumnToContents(column);
}
void iTableView::resizeColumnsToContents(){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	QTableView::resizeColumnsToContents();
}
void iTableView::sortByColumn(int column){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	QTableView::sortByColumn(column);
}
void iTableView::setShowGrid(bool show){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	QTableView::setShowGrid(show);
}

void iTableView::reset(){
	if(true) qDebug()<<Q_FUNC_INFO;
	QTableView::reset();

}

void iTableView::edit(const QModelIndex &index){
	if(true) qDebug()<<Q_FUNC_INFO;
	QTableView::edit(index);
}
void iTableView::setCurrentIndex(const QModelIndex &index){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	QTableView::setCurrentIndex(index);
}
void iTableView::update(const QModelIndex &index){
	if(true) qDebug()<<Q_FUNC_INFO;
	QTableView::update(index);
}

void iTableView::dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	QTableView::dataChanged(topLeft, bottomRight);
}
void iTableView::currentChanged(const QModelIndex &current, const QModelIndex &previous){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	QTableView::currentChanged(current, previous);
}
void iTableView::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	QTableView::selectionChanged(selected, deselected);
}

void iTableView::resizeEvent(QResizeEvent *event){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	ResizeColumns(this->width());
	if(adjustHeightToContent) AdjustHeightToContent();
	QTableView::resizeEvent(event);
}


void iTableView::timerEvent(QTimerEvent* evt){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	QTableView::timerEvent(evt);
}

void iTableView::paintEvent(QPaintEvent* evt){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	QTableView::paintEvent(evt);
}


/////////////////////////////////////////////////////////////////////



iHeaderView::iHeaderView(Qt::Orientation orientation, QWidget *parent)
	: QHeaderView(orientation, parent){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	setClickable(true);
	setItemDelegate(new iDelegate());
}

iHeaderView::~iHeaderView(){}

QMap<const iHeaderView*, bool> sectionArrow;

void iHeaderView::setFilterArrow(int section){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	FilterSection.insert(section);
	updateSection(section);
}

void iHeaderView::removeFilterArrow(int section){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	FilterSection.remove(section);
	updateSection(section);
}

void iHeaderView::clearFilterArrows(){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	FilterSection.clear();
	update();
}

void iHeaderView::initStyleOption(QStyleOptionHeader *option) const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	QHeaderView::initStyleOption(option);
	if(sectionArrow.contains(this)){
		option->sortIndicator = QStyleOptionHeader::SortDown;
		sectionArrow.remove(this);
		qDebug() << "looooooooool";
	}
}

void iHeaderView::paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	if (!rect.isValid())
		return;
	QStyleOptionHeader opt;
	initStyleOption(&opt);

	QStyle::State state = QStyle::State_None;
	if (isEnabled())
		state |= QStyle::State_Enabled;
	if (window()->isActiveWindow())
		state |= QStyle::State_Active;


	if(FilterSection.contains(logicalIndex))
		opt.sortIndicator =QStyleOptionHeader::SortDown;

	if (isSortIndicatorShown() && sortIndicatorSection() == logicalIndex)
		opt.sortIndicator = (sortIndicatorOrder() == Qt::AscendingOrder)
				? QStyleOptionHeader::SortDown : QStyleOptionHeader::SortUp;

	QVariant textAlignment = model()->headerData(logicalIndex, orientation(),
												 Qt::TextAlignmentRole);

	opt.rect = rect;
	opt.section = logicalIndex;
	opt.state |= state;
	opt.textAlignment = Qt::Alignment(textAlignment.isValid()
									  ? Qt::Alignment(textAlignment.toInt())
									  : defaultAlignment());

	opt.iconAlignment = Qt::AlignVCenter;
	opt.text = model()->headerData(logicalIndex, orientation(),
									Qt::DisplayRole).toString();
	int margin = 2 * style()->pixelMetric(QStyle::PM_HeaderMargin, 0, this);

	const Qt::Alignment headerArrowAlignment = static_cast<Qt::Alignment>(style()->styleHint(QStyle::SH_Header_ArrowAlignment, 0, this));
	const bool isHeaderArrowOnTheSide = headerArrowAlignment & Qt::AlignVCenter;
	if (isSortIndicatorShown() && sortIndicatorSection() == logicalIndex && isHeaderArrowOnTheSide)
		margin += style()->pixelMetric(QStyle::PM_HeaderMarkSize, 0, this);
	const QVariant variant = model()->headerData(logicalIndex, orientation(),
												  Qt::DecorationRole);
	opt.icon = qvariant_cast<QIcon>(variant);
	if (opt.icon.isNull())
		opt.icon = qvariant_cast<QPixmap>(variant);
	if (!opt.icon.isNull()) // see CT_HeaderSection
		margin += style()->pixelMetric(QStyle::PM_SmallIconSize, 0, this) +
				style()->pixelMetric(QStyle::PM_HeaderMargin, 0, this);
	if (textElideMode() != Qt::ElideNone) {
		const QRect textRect = style()->subElementRect(QStyle::SE_HeaderLabel, &opt, this);
		opt.text = opt.fontMetrics.elidedText(opt.text, textElideMode(), textRect.width() - margin);
	}
	QVariant foregroundBrush = model()->headerData(logicalIndex, orientation(),
													Qt::ForegroundRole);
	if (foregroundBrush.canConvert<QBrush>())
		opt.palette.setBrush(QPalette::ButtonText, qvariant_cast<QBrush>(foregroundBrush));
	QPointF oldBO = painter->brushOrigin();
	QVariant backgroundBrush = model()->headerData(logicalIndex, orientation(),
													Qt::BackgroundRole);
	if (backgroundBrush.canConvert<QBrush>()) {
		opt.palette.setBrush(QPalette::Button, qvariant_cast<QBrush>(backgroundBrush));
		opt.palette.setBrush(QPalette::Window, qvariant_cast<QBrush>(backgroundBrush));
		painter->setBrushOrigin(opt.rect.topLeft());
	}


	// the section position
	int visual = visualIndex(logicalIndex);
	Q_ASSERT(visual != -1);
	bool first = visual==0;
	bool last = visual==count() -1;
	if (first && last)
		opt.position = QStyleOptionHeader::OnlyOneSection;
	else if (first)
		opt.position = QStyleOptionHeader::Beginning;
	else if (last)
		opt.position = QStyleOptionHeader::End;
	else
		opt.position = QStyleOptionHeader::Middle;
	opt.orientation = orientation();
	style()->drawControl(QStyle::CE_Header, &opt, painter, this);
	painter->setBrushOrigin(oldBO);

}




























