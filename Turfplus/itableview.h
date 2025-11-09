#ifndef ITABLEVIEW_H
#define ITABLEVIEW_H

#include <QObject>
#include <QTableView>
#include <QHeaderView>
#include <QMap>
#include "idelegate.h"
#include "imodel.h"

class FlipModel;

class Q_GUI_EXPORT iHeaderView : public QHeaderView{
	Q_OBJECT
public:
	iHeaderView(Qt::Orientation orientation, QWidget *parent = 0);
	virtual ~iHeaderView();

	 void setFilterArrow(int section);
	 void removeFilterArrow(int section);
	 void clearFilterArrows();

protected:
	 virtual void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const;
	 void initStyleOption(QStyleOptionHeader *option) const;
private:
	 bool debugFlag = false;
	 QSet<int> FilterSection;

};

class Q_GUI_EXPORT iTableView : public QTableView{

	Q_OBJECT
protected:
	void setHorizontalHeader(iHeaderView *header);
	void setVerticalHeader(iHeaderView *header);
public:
	void setdebugFlag(bool flag);
	void setFixedColumnWidth(const QString&, int);
	void resetFixedColumnWidth();
	void setHideEmptyColumns(bool = true);
	void setFlipTable(bool flipFlag = true);
	void setadjustHeightToContent(bool = true);

	/***************************************************/
	iTableView(QWidget *parent = 0);
	virtual ~iTableView();

	void setModel(iModel *model);
	iModel* model() const;

	void setRootIndex(const QModelIndex &index);
	void setSelectionModel(QItemSelectionModel *selectionModel);
	void doItemsLayout();

	iHeaderView *horizontalHeader() const;
	iHeaderView *verticalHeader() const;

	int rowViewportPosition(int row) const;
	int rowAt(int y) const;

	void setRowHeight(int row, int height);
	int rowHeight(int row) const;

	int columnViewportPosition(int column) const;
	int columnAt(int x) const;

	void setColumnWidth(int column, int width);
	int columnWidth(int column) const;

	bool isRowHidden(int row) const;
	void setRowHidden(int row, bool hide);

	bool isColumnHidden(int column) const;
	void setColumnHidden(int column, bool hide);

	void setSortingEnabled(bool enable);
	bool isSortingEnabled() const;

	bool showGrid() const;



	Qt::PenStyle gridStyle() const;
	void setGridStyle(Qt::PenStyle style);

	void setWordWrap(bool on);
	bool wordWrap() const;

	void setCornerButtonEnabled(bool enable);
	bool isCornerButtonEnabled() const;

	QRect visualRect(const QModelIndex &index) const;
	void scrollTo(const QModelIndex &index, ScrollHint hint = EnsureVisible);
	QModelIndex indexAt(const QPoint &p) const;

	void setSpan(int row, int column, int rowSpan, int columnSpan);
	int rowSpan(int row, int column) const;
	int columnSpan(int row, int column) const;
	void clearSpans();

	void sortByColumn(int column, Qt::SortOrder order);


	iDelegate *itemDelegate() const;

	iDelegate *horizontalHeaderItemDelegate() const;
	iDelegate *verticalHeaderItemDelegate() const;

protected:
	virtual void paintEvent(QPaintEvent* evt) override;
	virtual void timerEvent(QTimerEvent* evt) override;

public Q_SLOTS:



	void selectRow(int row);
	void selectColumn(int column);

	void hideColumn(const QString& column);
	void showColumn(const QString& column);

	void hideRow(int row);
	void hideColumn(int column);
	void showRow(int row);
	void showColumn(int column);
	void resizeRowToContents(int row);
	void resizeRowsToContents();
	void resizeColumnToContents(int column);
	void resizeColumnsToContents();
	void sortByColumn(int column);
	void setShowGrid(bool show);

	virtual void reset();
	void edit(const QModelIndex &index);
	void setCurrentIndex(const QModelIndex &index);
	void update(const QModelIndex &index);


protected Q_SLOTS:
	void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
	void currentChanged(const QModelIndex &current, const QModelIndex &previous);
	void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
	void resizeEvent(QResizeEvent *event) override;

	/*****************************************/


public Q_SLOTS:
	void HideEmptyColumns();
	void ResizeColumns(int widht);
	void AdjustHeightToContent();
	void PopAllEditors();
	void contextFilterMenu(const QPoint &point);
	void repopHeaderMenu(QAction*);
	void resetFilterMenus();


private:
	bool hideEmptyColumns = true;
	bool debugFlag = false;
	bool loadingFalg = true;
	bool flipTable = false;
	bool adjustHeightToContent = false;
	FlipModel * flip = NULL;
	iModel*	source = NULL;
	QMap<QString, int> FixedColumnWidth;
	QSet<int> hiddenCols;
	QMap<QString, QMenu*> columnFilters;


	/*************************************************/


};

#endif // ITABLEVIEW_H
