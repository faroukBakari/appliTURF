#include "qitemselectionmodel.h"
#include <qsize.h>
#include <qstringlist.h>
#include <QDebug>
#include <QPixmap>
#include <QMovie>
#include <QPushButton>
#include <QLabel>
#include "QProgressIndicator.h"
#include <QSqlError>
#include <QAction>
#include <QMenu>
#include "imodel.h"
#include "itableview.h"

void iModel::setdebugFlag(bool flag){
	debugFlag = flag;
	qDebug()<<Q_FUNC_INFO;
	qDebug()<< "debugFlag = " << debugFlag;
}

void iModel::filterSlot(QAction* action){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	if(action->text() == "Clear_All")
		Filters[((QMenu *)action->parent())->title()].clear();
	if(action->isChecked())
		Filters[((QMenu *)action->parent())->title()].insert(action->text());
	else
		Filters[((QMenu *)action->parent())->title()].remove(action->text());
	invalidateFilter();
}

bool iModel::isFiltred(const QString& col, const QString& value){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return Filters.contains(col) && Filters[col].contains(value);
}

void iModel::addFilter(const QString& col, const QString& value){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	Filters[col].insert(value);
	invalidateFilter();
}
void iModel::removeFilter(const QString& col, const QString& value){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	Filters[col].remove(value);
	invalidateFilter();
}
void iModel::removeColumnFilters(const QString& col){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	Filters[col].clear();
	invalidateFilter();
}
void iModel::resetFilters(){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	Filters.clear();
}

void iModel::setTargetWidget(QWidget* w){
	TargetWidget = w;
}

void iModel::resetSourceColIdx(){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	SourceColIdx.clear();
	for(int col = 0; col < sourceModel()->columnCount(); col++){
		SourceColIdx.insert(sourceModel()->headerData(col,Qt::Horizontal).toString(), col);
		if(debugFlag) qDebug() << sourceModel()->headerData(col,Qt::Horizontal).toString() << " : " << col;
	}
}

void iModel::resetColIdx(){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	ColIdx.clear();
	for(int col = 0; col < columnCount(); col++){
		ColIdx.insert(headerData(col,Qt::Horizontal).toString(), col);
		if(debugFlag) qDebug() << headerData(col,Qt::Horizontal).toString() << " : " << col;
	}
}

void iModel::restFlags(){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	userFlags.clear();
}
void iModel::resetUserValues(){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	userValues.clear();
}

int iModel::sourceColIdx(const QString& col) const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	if(!SourceColIdx.contains(col)){
		qDebug() << Q_FUNC_INFO << "WARNING : column " <<col<< " not found";
		return -1;
	}
	return SourceColIdx.value(col);
}
int iModel::colIdx(const QString& col) const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	if(!ColIdx.contains(col)){
		qDebug() << Q_FUNC_INFO << "WARNING : column " <<col<< " not found";
		return -1;
	}
	return ColIdx.value(col);
}

QModelIndex iModel::SourceIndex(int row, int col) const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return sourceModel()->index(row, col);
}

QModelIndex iModel::SourceIndex(int row, const QString& col) const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return sourceModel()->index(row, SourceColIdx.value(col));
}

QVariant iModel::SourceData(int row, int col, int role) const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return SourceIndex(row, col).data(role);
}

QVariant iModel::SourceData(int row, const QString& col, int role) const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return SourceIndex(row, col).data(role);
}

void iModel::setFlag(int row, int col, Qt::ItemFlags flag){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	auto targetIdx = mapToSource(index(row, col));
	if(!userFlags.contains(targetIdx)) userFlags.insert(targetIdx, flag);
	else userFlags[targetIdx] |= flag;
	emit layoutChanged();
}

void iModel::setFlag(int row, const QString& col, Qt::ItemFlags flag){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	auto targetIdx = mapToSource(index(row, col));
	if(!userFlags.contains(targetIdx)) userFlags.insert(targetIdx, flag);
	else userFlags[targetIdx] |= flag;
	emit layoutChanged();
}

void iModel::clearFlag(int row, int col, Qt::ItemFlags flag){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	auto targetIdx = mapToSource(index(row, col));
	if(userFlags.contains(targetIdx)) userFlags[targetIdx] &= ~flag;
	emit layoutChanged();

}
void iModel::clearFlag(int row, const QString& col, Qt::ItemFlags flag){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	auto targetIdx = mapToSource(index(row, col));
	if(userFlags.contains(targetIdx)) userFlags[targetIdx] &= ~flag;
	emit layoutChanged();
}


QList<QVariant> iModel::columnData(int col, int role){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	QList<QVariant> out;
	for(int row = 0; row < rowCount(); row++){
		out << data(index(row, col),role);
	}
	return out;
}
QList<QVariant> iModel::columnData(const QString& col, int role){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	QList<QVariant> out;
	for(int row = 0; row < rowCount(); row++){
		out << data(index(row, col),role);
	}
	return out;
}

void iModel::setRowData(int row, const QVariant& data, int role){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;

	for(int col = 0; col < columnCount(); col++){
		if(debugFlag) qDebug() << "adding new row value : "
					<< data << " role =" << role
					<< " index = " << index(row,col);
		userValues[role].insert(mapToSource(index(row,col)), data);
	}

	emit dataChanged(index(row, 0), index(row, columnCount()));
}
void iModel::setRowData(int row, const QList<QVariant>& data, int role){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;

	int ln = data.length();
	for(int col = 0; col < columnCount(); col++){
		if(debugFlag) qDebug() << "adding new column value : "
					<< ((col < ln) ? data.at(col) : QVariant()) << " role =" << role
					<< " index = " << index(row,col);
		userValues[role].insert(mapToSource(index(row,col)), (col < ln) ? data.at(col) : QVariant());
	}

	emit dataChanged(index(row, 0), index(row, columnCount()));
}

void iModel::setColumnData(int col, const QVariant& data, int role){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;

	for(int row = 0; row < rowCount(); row++){
		if(debugFlag) qDebug() << "adding new column value : "
					<< data << " role =" << role
					<< " index = " << index(row,col);
		userValues[role].insert(mapToSource(index(row,col)), data);
	}

	emit dataChanged(index(0, col), index(rowCount(), col));
}

void iModel::setColumnData(const QString& col, const QVariant& data, int role){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;

	for(int row = 0; row < rowCount(); row++){
		if(debugFlag) qDebug() << "adding new column value : "
					<< data << " role =" << role
					<< " index = " << index(row,col);
		userValues[role].insert(mapToSource(index(row,col)), data);
	}

	emit dataChanged(index(0, col), index(rowCount(), col));
}

void iModel::setColumnData(int col, const QList<QVariant>& data, int role){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;

	int ln = data.length();
	for(int row = 0; row < rowCount(); row++){
		if(debugFlag) qDebug() << "adding new column value : "
					<< ((row < ln) ? data.at(row) : QVariant()) << " role =" << role
					<< " index = " << index(row,col);
		userValues[role].insert(mapToSource(index(row,col)), (row < ln) ? data.at(row) : QVariant());
	}

	emit dataChanged(index(0, col), index(rowCount(), col));
}
void iModel::setColumnData(const QString& colName, const QList<QVariant>& data, int role){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;

	int col = colIdx(colName), ln = data.length();

	for(int row = 0; row < rowCount(); row++){
		if(debugFlag) qDebug() << "adding new column value : "
					<< ((row < ln) ? data.at(row) : QVariant()) << " role =" << role
					<< " index = " << index(row,col);
		userValues[role].insert(mapToSource(index(row,col)), (row < ln) ? data.at(row) : QVariant());
	}

	emit dataChanged(index(0, col), index(rowCount(), col));
}

bool iModel::addNewColumn(const QString& colName, const QList<QVariant>& data, int column, const QModelIndex &parent){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	if(column < 0) column = columnCount();
	bool out = QSortFilterProxyModel::insertColumns(column, 1, parent);
	setHeaderData(column,Qt::Horizontal, colName);
	setColumnData(colName, data);
	emit layoutChanged();
	return out;
}


void iModel::setColumnFlag(int col, Qt::ItemFlags flag){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	QModelIndex targetIdx;
	for(int row = 0; row < rowCount(); row++){
		targetIdx = mapToSource(index(row, col));
		if(!userFlags.contains(targetIdx)) userFlags.insert(targetIdx, flag);
		else userFlags[targetIdx] |= flag;
	}
	emit layoutChanged();
}

void iModel::setColumnFlag(const QString& col, Qt::ItemFlags flag){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	QModelIndex targetIdx;
	for(int row = 0; row < rowCount(); row++){
		targetIdx = mapToSource(index(row, col));
		if(!userFlags.contains(targetIdx)) userFlags.insert(targetIdx, flag);
		else userFlags[targetIdx] |= flag;
	}
	emit layoutChanged();
}

void iModel::clearColumnFlag(int col, Qt::ItemFlags flag){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	QModelIndex targetIdx;
	for(int row = 0; row < rowCount(); row++){
		targetIdx = mapToSource(index(row, col));
		if(userFlags.contains(targetIdx)) userFlags[targetIdx] &= ~flag;
	}
	emit layoutChanged();
}

void iModel::clearColumnFlag(const QString& col, Qt::ItemFlags flag){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	QModelIndex targetIdx;
	for(int row = 0; row < rowCount(); row++){
		targetIdx = mapToSource(index(row, col));
		if(userFlags.contains(targetIdx)) userFlags[targetIdx] &= ~flag;
	}
	emit layoutChanged();
}

void iModel::clearUsrData(int row, int col, int role){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	QModelIndex targetIdx = index(row, col);
	if(userValues.contains(role)) userValues[role].remove(mapToSource(targetIdx));
	emit dataChanged(targetIdx, targetIdx);
}
void iModel::clearUsrData(int row, const QString& col, int role){
	QModelIndex targetIdx = index(row, col);
	if(userValues.contains(role)) userValues[role].remove(mapToSource(targetIdx));
	emit dataChanged(targetIdx, targetIdx);
}

void iModel::clearUsrColumnData(int col, int role){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	for(int row = 0; row < rowCount(); row++)
		if(userValues.contains(role)) userValues[role].remove(mapToSource(index(row, col)));
	emit dataChanged(index(0, col), index(rowCount(), col));
}
void iModel::clearUsrColumnData(const QString& col, int role){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	for(int row = 0; row < rowCount(); row++)
		if(userValues.contains(role)) userValues[role].remove(mapToSource(index(row, col)));
	emit dataChanged(index(0, col), index(rowCount(), col));
}


/**************************************************/

iModel::iModel(QObject *parent) : QSortFilterProxyModel(parent){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	qDebug()<< "debugFlag = " << debugFlag;
	setSourceModel(new QSqlQueryModel(parent));
}

iModel::~iModel(){
	if(loading) delete loading;
}

void iModel::setSourceModel(QAbstractItemModel* sourceModel){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	QSortFilterProxyModel::setSourceModel(sourceModel);
	resetSourceColIdx();
	resetColIdx();
	resetUserValues();
	restFlags();
	emit dataAvalable(true);
	emit layoutChanged();
	emit dataChanged(index(0,0), index(rowCount(), columnCount()));
}

QSqlQueryModel* iModel::sourceModel() const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return static_cast<QSqlQueryModel*>(QAbstractProxyModel::sourceModel());
}

void iModel::setQueryAsync(const QString& query){
	SqlQueryThread *workerThread = new SqlQueryThread(this, query);
	connect(workerThread, SIGNAL(resultReady(QSqlQuery*)), this, SLOT(setQueryResults(QSqlQuery*)) );
	connect(workerThread, SIGNAL(finished()), workerThread, SLOT(deleteLater()));
	qDebug() << "setQuery...";
	emit dataAvalable(false);
	resetFilters();
	if(!loading) loading = new LoadingDialog(TargetWidget);
    if(TargetWidget){
        loading->move((TargetWidget->rect().topLeft() +
                         TargetWidget->rect().bottomRight())/2 -
                        (loading->rect().topLeft() +
                         loading->rect().bottomRight())/2);
    }

	loading->show();
	workerThread->start();
}

int errSql = 0;

void iModel::setQueryResults(QSqlQuery* query){
	if(true) qDebug()<<Q_FUNC_INFO;
	if(query->lastError().number() != -1){
		if(errSql > 3){
			qDebug() << "MULTIPLE FAILS FOR QUERY -> Aborting";
			abort();
		}
		qDebug() << "Async SqlQuery Error : " << query->lastError();
		QSqlDatabase db = QSqlDatabase::database();
		db.close();
		db.open();
		errSql++;
		setQueryAsync(query->lastQuery());
		query->clear();
		free(query);
		return;
	}
	errSql = 0;
	sourceModel()->setQuery(*query);
	query->clear(); free(query);
	qDebug() << "Query done!";
	loading->close();
	resetSourceColIdx();
	resetColIdx();
	resetUserValues();
	restFlags();
	emit layoutChanged();
	emit dataChanged(index(0,0), index(rowCount(), columnCount()));
	emit dataAvalable(true);

}

void iModel::setQuery(const QString& query){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	resetFilters();
	emit dataAvalable(false);
	QSqlQuery qr(query);
	while(qr.lastError().number() != -1){
		if(errSql > 3){
			qDebug() << "MULTIPLE FAILS FOR QUERY -> Aborting";
			abort();
		}
		qDebug() << "SqlQuery Error : " << qr.lastError();
		QSqlDatabase db = QSqlDatabase::database();
		db.close();
		db.open();
		qr = db.exec(query);
	}
	errSql = 0;
	sourceModel()->setQuery(qr);
	resetSourceColIdx();
	resetColIdx();
	resetUserValues();
	restFlags();
	emit layoutChanged();
	emit dataChanged(index(0,0), index(rowCount(), columnCount()));
	emit dataAvalable(true);
}

void iModel::setQuery(const QSqlQuery& query){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	sourceModel()->setQuery(query);
	resetFilters();
	resetSourceColIdx();
	resetColIdx();
	resetUserValues();
	restFlags();
	emit layoutChanged();
	emit dataChanged(index(0,0), index(rowCount(), columnCount()));
	emit dataAvalable(true);
}

QModelIndex iModel::mapToSource(const QModelIndex &proxyIndex) const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return QSortFilterProxyModel::mapToSource(proxyIndex);
}
QModelIndex iModel::mapFromSource(const QModelIndex &sourceIndex) const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return QSortFilterProxyModel::mapFromSource(sourceIndex);
}

QItemSelection iModel::mapSelectionToSource(const QItemSelection &proxySelection) const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return QSortFilterProxyModel::mapSelectionToSource(proxySelection);
}
QItemSelection iModel::mapSelectionFromSource(const QItemSelection &sourceSelection) const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return QSortFilterProxyModel::mapSelectionFromSource(sourceSelection);
}

QRegExp iModel::filterRegExp() const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return QSortFilterProxyModel::filterRegExp();
}
void iModel::setFilterRegExp(const QRegExp &regExp){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return QSortFilterProxyModel::setFilterRegExp(regExp);
}

int iModel::filterKeyColumn() const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return QSortFilterProxyModel::filterKeyColumn();
}
void iModel::setFilterKeyColumn(int column){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return QSortFilterProxyModel::setFilterKeyColumn(column);
}

Qt::CaseSensitivity iModel::filterCaseSensitivity() const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return QSortFilterProxyModel::filterCaseSensitivity();
}
void iModel::setFilterCaseSensitivity(Qt::CaseSensitivity cs){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return QSortFilterProxyModel::setFilterCaseSensitivity(cs);
}

Qt::CaseSensitivity iModel::sortCaseSensitivity() const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return QSortFilterProxyModel::sortCaseSensitivity();
}
void iModel::setSortCaseSensitivity(Qt::CaseSensitivity cs){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return QSortFilterProxyModel::setSortCaseSensitivity(cs);
}

bool iModel::isSortLocaleAware() const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return QSortFilterProxyModel::isSortLocaleAware();
}
void iModel::setSortLocaleAware(bool on){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return QSortFilterProxyModel::setSortLocaleAware(on);
}

int iModel::sortColumn() const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return QSortFilterProxyModel::sortColumn();
}
Qt::SortOrder iModel::sortOrder() const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return QSortFilterProxyModel::sortOrder();
}

bool iModel::dynamicSortFilter() const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return QSortFilterProxyModel::dynamicSortFilter();
}
void iModel::setDynamicSortFilter(bool enable){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return QSortFilterProxyModel::setDynamicSortFilter(enable);
}

int iModel::sortRole() const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return QSortFilterProxyModel::sortRole();
}
void iModel::setSortRole(int role){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return QSortFilterProxyModel::setSortRole(role);
}

int iModel::filterRole() const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return QSortFilterProxyModel::filterRole();
}
void iModel::setFilterRole(int role){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return QSortFilterProxyModel::setFilterRole(role);
}


void iModel::setFilterRegExp(const QString &pattern){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	QSortFilterProxyModel::setFilterRegExp(pattern);
}
void iModel::setFilterWildcard(const QString &pattern){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	QSortFilterProxyModel::setFilterWildcard(pattern);
}
void iModel::setFilterFixedString(const QString &pattern){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	QSortFilterProxyModel::setFilterFixedString(pattern);
}
void iModel::clear(){
	if(true) qDebug()<<Q_FUNC_INFO;
	QSortFilterProxyModel::clear();
}
void iModel::invalidate(){
	if(true) qDebug()<<Q_FUNC_INFO;
	QSortFilterProxyModel::invalidate();
}

bool iModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;

	for(const QString& col : Filters.keys()){
		if(!Filters[col].empty()){
			bool out = false;
			for(const QString& val : Filters[col]){
				if(val == SourceData(source_row, col)){
					out = true;
					break;
				}
			}
			if(!out) return false;
		}
	}

	return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
}
bool iModel::filterAcceptsColumn(int source_column, const QModelIndex &source_parent) const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return QSortFilterProxyModel::filterAcceptsColumn(source_column, source_parent);
}
bool iModel::lessThan(const QModelIndex &left, const QModelIndex &right) const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return QSortFilterProxyModel::lessThan(left, right);
}

void iModel::invalidateFilter(){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	QSortFilterProxyModel::invalidateFilter();
	emit layoutChanged();
}

inline QObject *iModel::parent() const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return QObject::parent();
}

QModelIndex iModel::index(int row, const QString& col, const QModelIndex &parent) const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return QSortFilterProxyModel::index(row, colIdx(col), parent);
}

QModelIndex iModel::index(int row, int column, const QModelIndex &parent) const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return QSortFilterProxyModel::index(row, column, parent);
}

QModelIndex iModel::parent(const QModelIndex &child) const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return QSortFilterProxyModel::parent(child);
}

int iModel::rowCount(const QModelIndex &parent) const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return QSortFilterProxyModel::rowCount(parent);
}
int iModel::columnCount(const QModelIndex &parent) const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return QSortFilterProxyModel::columnCount(parent);
}
bool iModel::hasChildren(const QModelIndex &parent) const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return QSortFilterProxyModel::hasChildren(parent);
}

QVariant iModel::data(int row, int col, int role) const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return data(index(row,col), role);
}

QVariant iModel::data(int row, const QString& col, int role) const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return data(index(row,col), role);
}

QVariant iModel::data(const QModelIndex &idx, int role) const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;

	QVariant out;

	if(userValues.contains(role) && userValues.value(role).contains(mapToSource(idx))){
		out = userValues.value(role)[mapToSource(idx)];
		if(debugFlag) qDebug() << "Applying user value" << out
							   << "(role :" << role
							   << ") to index" << idx;
	}
	else out = QSortFilterProxyModel::data(idx, role);

	if(out.type() == QVariant::ByteArray && (role == Qt::DisplayRole || role == Qt::EditRole)){
		if(debugFlag) qDebug() << "iModel::data --> Warning QByteArray type in index" << idx << "(use Qt::DecorationRole to get QPixmap)";
		out = QVariant("");
	}

	if(role == Qt::DecorationRole){
		QVariant variant(QSortFilterProxyModel::data(idx, Qt::DisplayRole));
		if(variant.type() == QVariant::ByteArray){
			QPixmap p;
			QByteArray bytes(variant.toByteArray());
			if(bytes.size() > 0){
				if(!p.loadFromData(bytes)) qDebug() << " Error loading Blob image from DB!!! column : " << headerData(idx.column(), Qt::Horizontal);
				out = QVariant(p);
				if(debugFlag) qDebug() << "col : "		<< idx.column()  // Gives Role 0 QVariant Type Qvariant(QByteArray)
									   << " - Role : "	<< role
									   << " - Type "	<< out.type()
									   << " value : "	<< out;
			}
			else out = QVariant();
		}
	}

	return out;
}

bool iModel::setData(int row, int col, const QVariant& value, int role){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return setData(index(row, col), value, role);
}

bool iModel::setData(int row, const QString& col, const QVariant& value, int role){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return setData(index(row, col), value, role);
}

bool iModel::setData(const QModelIndex &targetIdx, const QVariant &value, int role){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	userValues[role].insert(mapToSource(targetIdx), value);
	emit dataChanged(targetIdx, targetIdx);
	return true;

}

QVariant iModel::headerData(const QString& colName, Qt::Orientation orientation, int role) const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	int section = colIdx(colName);
	return QSortFilterProxyModel::headerData(section,orientation,role);
}


bool iModel::setHeaderData(const QString& colName, Qt::Orientation orientation, const QVariant &value, int role){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	int section = colIdx(colName);
	bool out = QSortFilterProxyModel::setHeaderData(section, orientation, value, role);
	resetSourceColIdx();
	resetColIdx();
	emit headerDataChanged(orientation, section, section);
	return out;

}

QVariant iModel::headerData(int section, Qt::Orientation orientation, int role) const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return QSortFilterProxyModel::headerData(section,orientation,role);
}


bool iModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	bool out = QSortFilterProxyModel::setHeaderData(section, orientation, value, role);
	resetSourceColIdx();
	resetColIdx();
	emit headerDataChanged(orientation, section, section);
	return out;

}

QMimeData *iModel::mimeData(const QModelIndexList &indexes) const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return QSortFilterProxyModel::mimeData(indexes);
}
bool iModel::dropMimeData(const QMimeData *data, Qt::DropAction action,
				  int row, int column, const QModelIndex &parent){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return QSortFilterProxyModel::dropMimeData(data,action,row,column,parent);
}

bool iModel::insertRows(int row, int count, const QModelIndex &parent){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	bool out = QSortFilterProxyModel::insertRows(row, count, parent);
	return out;
}
bool iModel::insertColumns(int column, int count, const QModelIndex &parent){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	bool out = QSortFilterProxyModel::insertColumns(column, count, parent);
	return out;
}
bool iModel::removeRows(int row, int count, const QModelIndex &parent){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	bool out = QSortFilterProxyModel::removeRows(row, count, parent);
	return out;
}
bool iModel::removeColumns(int column, int count, const QModelIndex &parent){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	bool out = QSortFilterProxyModel::removeColumns(column, count, parent);
	return out;
}

void iModel::fetchMore(const QModelIndex &parent){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return QSortFilterProxyModel::fetchMore(parent);
}
bool iModel::canFetchMore(const QModelIndex &parent) const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return QSortFilterProxyModel::canFetchMore(parent);
}
Qt::ItemFlags iModel::flags(const QModelIndex &index) const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	Qt::ItemFlags flag = QSortFilterProxyModel::flags(index);

	auto sourceIdx = mapToSource(index);
	if(userFlags.contains(sourceIdx)){
		if(debugFlag) qDebug() << "iModel::flags -> custom flag :" << flag
							   << "for index :" << index;
		flag |= userFlags.value(sourceIdx);
	}

	return flag;
}


QModelIndex iModel::buddy(const QModelIndex &index) const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return QSortFilterProxyModel::buddy(index);
}
QModelIndexList iModel::match(const QModelIndex &start, int role,
					  const QVariant &value, int hits,
					  Qt::MatchFlags flags) const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return QSortFilterProxyModel::match(start, role, value, hits, flags);
}
QSize iModel::span(const QModelIndex &index) const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return QSortFilterProxyModel::span(index);
}
void iModel::sort(int column, Qt::SortOrder order){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return QSortFilterProxyModel::sort(column, order);
}

QStringList iModel::mimeTypes() const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return QSortFilterProxyModel::mimeTypes();
}
Qt::DropActions iModel::supportedDropActions() const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return QSortFilterProxyModel::supportedDropActions();
}

/*********************************************/
//From QAbstractItemModel












