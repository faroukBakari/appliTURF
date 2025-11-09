#ifndef IMODEL_H
#define IMODEL_H

#include <QSqlQueryModel>
#include <QSortFilterProxyModel>
#include <QObject>
#include <QMessageBox>
#include <QThread>
#include <QSqlQuery>
#include <QLabel>
#include <QSplashScreen>
#include <QSet>
#include <loadingdialog.h>

class iTableView;


class SqlQueryThread : public QThread{
	Q_OBJECT
public:
	SqlQueryThread(QObject* parent, const QString& Query)
		: QThread(parent), Query(Query){}
	void run() override {
		QSqlQuery *result = new QSqlQuery(Query);
		emit resultReady(result);
	}
signals:
	void resultReady(QSqlQuery* result);
private:
	QString Query;
};


class iModel : public QSortFilterProxyModel{

	Q_OBJECT

	friend class iTableView;

Q_SIGNALS:
	void dataAvalable(bool);

public:

	QSqlQueryModel* sourceModel() const;
	void setSourceModel(QAbstractItemModel *sourceModel);

	void setdebugFlag(bool flag);
	void setTargetWidget(QWidget*);

	void addFilter(const QString& col, const QString& value);
	void removeFilter(const QString& col, const QString& value);
	void removeColumnFilters(const QString& col);
	void resetFilters();

	bool isFiltred(const QString& col, const QString& value);

	int sourceColIdx(const QString& col) const;
	int colIdx(const QString& col) const;

	QModelIndex SourceIndex(int row, int col) const;
	QModelIndex SourceIndex(int row, const QString& col) const;

	QVariant SourceData(int row, int col, int role = Qt::DisplayRole) const;
	QVariant SourceData(int row, const QString& col, int role = Qt::DisplayRole) const;

	void setFlag(int row, int col, Qt::ItemFlags flag);
	void setFlag(int row, const QString& col, Qt::ItemFlags flag);

	void clearFlag(int row, int col, Qt::ItemFlags flag);
	void clearFlag(int row, const QString& col, Qt::ItemFlags flag);

	QModelIndex index(int row, const QString& col, const QModelIndex &parent = QModelIndex()) const;
	QVariant data(int row, int col, int role = Qt::DisplayRole) const;
	QVariant data(int row, const QString& col, int role = Qt::DisplayRole) const;

	bool setData(int row, int col, const QVariant& value, int role);
	bool setData(int row, const QString& col, const QVariant& value, int role);

	QList<QVariant> columnData(int col, int role = Qt::DisplayRole);
	QList<QVariant> columnData(const QString& col, int role = Qt::DisplayRole);

	template <class T>
	QSet<T> columnUniqData(int col, int role = Qt::DisplayRole){
		if(debugFlag) qDebug()<<Q_FUNC_INFO;
		QSet<T> out;
		for(int row = 0; row < rowCount(); row++){
			out << data(index(row, col),role).value<T>();
		}
		return out;
	}

	template <class T>
	QSet<T> columnUniqData(const QString& col, int role = Qt::DisplayRole){
		if(debugFlag) qDebug()<<Q_FUNC_INFO;
		QSet<T> out;
		for(int row = 0; row < rowCount(); row++){
			out << data(index(row, col),role).value<T>();
		}
		return out;
	}

	void setColumnData(int col, const QVariant& data, int role = Qt::DisplayRole);
	void setColumnData(int col, const QList<QVariant>& data, int role = Qt::DisplayRole);
	void setColumnData(const QString& col, const QVariant& data, int role = Qt::DisplayRole);
	void setColumnData(const QString& col, const QList<QVariant>& data, int role = Qt::DisplayRole);

	void setRowData(int row, const QVariant& data, int role = Qt::DisplayRole);
	void setRowData(int row, const QList<QVariant>& data, int role = Qt::DisplayRole);

	bool addNewColumn(const QString& colName, const QList<QVariant>& data = QList<QVariant>(), int column = -1, const QModelIndex &parent = QModelIndex());

	void setColumnFlag(int col, Qt::ItemFlags flag);
	void setColumnFlag(const QString& col, Qt::ItemFlags flag);


	void clearColumnFlag(int col, Qt::ItemFlags flag);
	void clearColumnFlag(const QString& col, Qt::ItemFlags flag);


	void clearUsrData(int row, int col, int role = Qt::DisplayRole);
	void clearUsrData(int row, const QString& col, int role = Qt::DisplayRole);

	void clearUsrColumnData(int col, int role = Qt::DisplayRole);
	void clearUsrColumnData(const QString& col, int role = Qt::DisplayRole);


public Q_SLOTS:
	void resetSourceColIdx();
	void resetColIdx();
	void restFlags();
	void resetUserValues();
	void setQueryAsync(const QString& query);
	void setQueryResults(QSqlQuery* query);
	void setQuery(const QString& query);
	void setQuery(const QSqlQuery& query);

	void filterSlot(QAction*);


	/***********************************/
public:
	iModel(QObject *parent = 0);
	virtual ~iModel();

	QModelIndex mapToSource(const QModelIndex &proxyIndex) const;
	QModelIndex mapFromSource(const QModelIndex &sourceIndex) const;

	QItemSelection mapSelectionToSource(const QItemSelection &proxySelection) const;
	QItemSelection mapSelectionFromSource(const QItemSelection &sourceSelection) const;

	QRegExp filterRegExp() const;
	void setFilterRegExp(const QRegExp &regExp);

	int filterKeyColumn() const;
	void setFilterKeyColumn(int column);

	Qt::CaseSensitivity filterCaseSensitivity() const;
	void setFilterCaseSensitivity(Qt::CaseSensitivity cs);

	Qt::CaseSensitivity sortCaseSensitivity() const;
	void setSortCaseSensitivity(Qt::CaseSensitivity cs);

	bool isSortLocaleAware() const;
	void setSortLocaleAware(bool on);

	int sortColumn() const;
	Qt::SortOrder sortOrder() const;

	bool dynamicSortFilter() const;
	void setDynamicSortFilter(bool enable);

	int sortRole() const;
	void setSortRole(int role);

	int filterRole() const;
	void setFilterRole(int role);

public Q_SLOTS:

	void setFilterRegExp(const QString &pattern);
	void setFilterWildcard(const QString &pattern);
	void setFilterFixedString(const QString &pattern);
	void clear();
	void invalidate();


protected:
	virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
	virtual bool filterAcceptsColumn(int source_column, const QModelIndex &source_parent) const;
	virtual bool lessThan(const QModelIndex &left, const QModelIndex &right) const;

	void invalidateFilter();

public:
	inline QObject *parent() const;

	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex &child) const;

	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;
	bool hasChildren(const QModelIndex &parent = QModelIndex()) const;

	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::DisplayRole);

	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::DisplayRole);

	QVariant headerData(const QString& section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	bool setHeaderData(const QString& section, Qt::Orientation orientation, const QVariant &value, int role = Qt::DisplayRole);

	QMimeData *mimeData(const QModelIndexList &indexes) const;
	bool dropMimeData(const QMimeData *data, Qt::DropAction action,
					   int row, int column, const QModelIndex &parent);

	bool insertRows(int row, int count = 1, const QModelIndex &parent = QModelIndex());
	bool insertColumns(int column, int count = 1, const QModelIndex &parent = QModelIndex());
	bool removeRows(int row, int count = 1, const QModelIndex &parent = QModelIndex());
	bool removeColumns(int column, int count = 1, const QModelIndex &parent = QModelIndex());

	void fetchMore(const QModelIndex &parent);
	bool canFetchMore(const QModelIndex &parent) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;

	QModelIndex buddy(const QModelIndex &index) const;
	QModelIndexList match(const QModelIndex &start, int role,
						   const QVariant &value, int hits = 1,
						   Qt::MatchFlags flags =
						   Qt::MatchFlags(Qt::MatchStartsWith|Qt::MatchWrap)) const;

	QSize span(const QModelIndex &index) const;
	void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);
	QStringList mimeTypes() const;
	Qt::DropActions supportedDropActions() const;

private:
	bool debugFlag = false;
	QMap<QString, int> SourceColIdx;
	QMap<QString, int> ColIdx;

	QMap<QModelIndex, Qt::ItemFlags> userFlags;
	QMap<int, QMap<QModelIndex, QVariant>>	userValues;

	QWidget* TargetWidget = NULL;
	LoadingDialog* loading = NULL;

	QMap<QString, QSet<QString>> Filters;
};

#endif // IMODEL_H

























