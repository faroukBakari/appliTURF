#ifndef IDELEGATE_H
#define IDELEGATE_H

#include <QObject>
#include <QStyledItemDelegate>
#include <QMap>

class QItemEditorFactory;

class iDelegate : public QStyledItemDelegate{
	Q_OBJECT
public:

	void setdebugFlag(bool flag);

	void addColStyle(const QString& colName, const QString& editor, const QList<QVariant>& data = QList<QVariant>());

	QString usrStyle(const QModelIndex &index) const;
	void clear();

	/****************************************************/
	explicit iDelegate(QObject *parent = 0);

	// painting
	void paint(QPainter *painter,
			   const QStyleOptionViewItem &option, const QModelIndex &index) const;
	QSize sizeHint(const QStyleOptionViewItem &option,
				   const QModelIndex &index) const;

	// editing
	QWidget *createEditor(QWidget *parent,
						  const QStyleOptionViewItem &option,
						  const QModelIndex &index) const;

	void setEditorData(QWidget *editor, const QModelIndex &index) const;
	void setModelData(QWidget *editor,
					  QAbstractItemModel *model,
					  const QModelIndex &index) const;

	void updateEditorGeometry(QWidget *editor,
							  const QStyleOptionViewItem &option,
							  const QModelIndex &index) const;

	// editor factory
	QItemEditorFactory *itemEditorFactory() const;
	void setItemEditorFactory(QItemEditorFactory *factory);

	virtual QString displayText(const QVariant &value, const QLocale &locale) const;

protected:
	virtual void initStyleOption(QStyleOptionViewItem *option,
								const QModelIndex &index) const;

	bool eventFilter(QObject *object, QEvent *event);
	bool editorEvent(QEvent *event, QAbstractItemModel *model,
					 const QStyleOptionViewItem &option, const QModelIndex &index);
private:
	bool debugFlag = false;
	QMap<QString, QString> UserColStyle;
	QMap<QString, QList<QVariant>> UserEditData;

};

#endif // IDELEGATE_H




















