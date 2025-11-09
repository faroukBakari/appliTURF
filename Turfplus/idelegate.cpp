#include <QtGlobal>
#include <QtGui>
#include <QtCore>
#include "idelegate.h"
#include <qsize.h>
#include <qstringlist.h>
#include "imodel.h"
#include <QStyledItemDelegate>

void iDelegate::setdebugFlag(bool flag){
	debugFlag = flag;
	qDebug()<<Q_FUNC_INFO;
	qDebug()<< "debugFlag = " << debugFlag;
}


void iDelegate::addColStyle(const QString& colName,const QString& editor, const QList<QVariant>& data){
	UserColStyle.insert(colName, editor);
	if(!data.empty()) UserEditData.insert(colName, data);
}

QString iDelegate::usrStyle(const QModelIndex &index) const{
	if(index.isValid()){
		QString colName(index.model()->headerData(index.column(), Qt::Horizontal).toString());
		if(UserColStyle.contains(colName))
			return UserColStyle.value(colName);
	}

	return QString();

}

void iDelegate::clear(){
	UserColStyle.clear();
}


/******************************************************************/

iDelegate::iDelegate(QObject *parent) : QStyledItemDelegate(parent){}

void iDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;


	if(!index.data().isNull() && UserColStyle.contains(index.model()->headerData(index.column(),Qt::Horizontal).toString())){
		QString w = UserColStyle.value(index.model()->headerData(index.column(),
									Qt::Horizontal).toString());

		if(w == "QProgressBar"){
			QStyleOptionProgressBar progressBarOption;
			progressBarOption.rect = option.rect;

			int progress = index.data().toInt();
			progressBarOption.minimum = INT_MAX;
			int val = 0;
			for(int i = 0; i < index.model()->rowCount(); i++){
				val = index.model()->data(index.model()->index(i,index.column())).toInt();
				if(progressBarOption.minimum > val) progressBarOption.minimum = val;
				if(progressBarOption.maximum < val) progressBarOption.maximum = val;
			}


			progressBarOption.textAlignment = Qt::AlignCenter;
			progressBarOption.progress = progress;
			progressBarOption.text = QString("%1").arg(index.data().toDouble());
			progressBarOption.textVisible = true;
			QApplication::style()->drawControl(QStyle::CE_ProgressBar, &progressBarOption, painter);
		}

		if(w == "QTextDocument"){
			QStyleOptionViewItem options = option;
			initStyleOption(&options, index);

			painter->save();

			QTextDocument doc;
			QTextOption textOption(doc.defaultTextOption());
			textOption.setWrapMode(QTextOption::WordWrap);
			doc.setDefaultTextOption(textOption);
			doc.setHtml(index.data().toString());

			doc.setTextWidth(options.rect.width());

			QWidget* prt = (QWidget*) parent();

			prt->style()->drawControl(QStyle::CE_ItemViewItem, &option, painter);

			painter->translate(options.rect.left(), options.rect.top());
			QRect clip(0, 0, options.rect.width(), options.rect.height());
			doc.drawContents(painter, clip);

			painter->restore();
		}


		return;

	}

	QStyledItemDelegate::paint(painter, option, index);
}

QSize iDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;

	if(!index.data().isNull() && UserColStyle.contains(index.model()->headerData(index.column(),Qt::Horizontal).toString())){
		QString w = UserColStyle.value(index.model()->headerData(index.column(),
									Qt::Horizontal).toString());
		if(w == "QTextDocument"){
			QStyleOptionViewItemV4 options = option;
			initStyleOption(&options, index);

			QTextDocument doc;
			doc.setHtml(index.data().toString());
			doc.setTextWidth(options.rect.width());
			return QSize(doc.idealWidth(), doc.size().height());
		}


	}
	return QStyledItemDelegate::sizeHint(option, index);
}

QWidget *iDelegate::createEditor(QWidget *parent,
					  const QStyleOptionViewItem & option,
					  const QModelIndex & index) const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;

	if(UserColStyle.contains(index.model()->headerData(index.column(),Qt::Horizontal).toString())){
		QString w = UserColStyle.value(index.model()->headerData(index.column(),
									Qt::Horizontal).toString());

		if(w == "QComboBox"){
			QComboBox* combo = new QComboBox(parent);
			return combo;
		}

	}

	return QStyledItemDelegate::createEditor(parent, option, index);
}



void iDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;

	if(UserColStyle.contains(index.model()->headerData(index.column(),Qt::Horizontal).toString())){
		QString w = UserColStyle.value(index.model()->headerData(index.column(),
									Qt::Horizontal).toString());

		if(w == "QComboBox"){
			QComboBox *combo = static_cast<QComboBox*>(editor);

			if(!combo->count()){

				if(UserEditData.contains(index.model()->headerData(index.column(),Qt::Horizontal).toString())){

					QList<QVariant> L = UserEditData.value(index.model()->headerData(index.column(),
												Qt::Horizontal).toString());

					QSet<QString> comboItems;
					for(auto& e : L)
						comboItems.insert(e.toString());
					QStringList comboList = comboItems.toList();
					comboList.sort();

					combo->clear();
					combo->addItems(comboList);
				}
				else
					qDebug() << "Warning : No selection items for QComboBox at " << index;
			}

			return;
		}

	}

	QStyledItemDelegate::setEditorData(editor, index);
}
void iDelegate::setModelData(QWidget *editor,
				  QAbstractItemModel *model,
				  const QModelIndex &index) const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;

	if(UserColStyle.contains(index.model()->headerData(index.column(),Qt::Horizontal).toString())){
		QString w = UserColStyle.value(index.model()->headerData(index.column(),
									Qt::Horizontal).toString());

		if(w == "QComboBox"){
			QComboBox *combo = static_cast<QComboBox*>(editor);
			model->setData(index, combo->currentText(), Qt::DisplayRole);
			return;
		}

		qDebug() << "unknown editor wideget type : " << w;
	}

	QStyledItemDelegate::setModelData(editor, model, index);
}

void iDelegate::updateEditorGeometry(QWidget *editor,
						  const QStyleOptionViewItem &option,
						  const QModelIndex &index) const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return QStyledItemDelegate::updateEditorGeometry(editor, option, index);
}

// editor factory
QItemEditorFactory *iDelegate::itemEditorFactory() const{
	qDebug()<<Q_FUNC_INFO;
	return QStyledItemDelegate::itemEditorFactory();
}
void iDelegate::setItemEditorFactory(QItemEditorFactory *factory){
	qDebug()<<Q_FUNC_INFO;
	return QStyledItemDelegate::setItemEditorFactory(factory);
}

QString iDelegate::displayText(const QVariant &value, const QLocale &locale) const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return QStyledItemDelegate::displayText(value, locale);
}

void iDelegate::initStyleOption(QStyleOptionViewItem *option,
							const QModelIndex &index) const{
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	return QStyledItemDelegate::initStyleOption(option, index);
}

bool iDelegate::eventFilter(QObject *object, QEvent *event){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;
	bool filter_val = QStyledItemDelegate::eventFilter(object, event);
	return filter_val;
}
bool iDelegate::editorEvent(QEvent *event, QAbstractItemModel *model,
				 const QStyleOptionViewItem &option, const QModelIndex &index){
	if(debugFlag) qDebug()<<Q_FUNC_INFO;

	bool filter_val = QStyledItemDelegate::editorEvent(event, model, option, index);
	static int eventEnumIndex = QEvent::staticMetaObject.indexOfEnumerator("Type");

	if(debugFlag){
		qDebug() << "event : " << QEvent::staticMetaObject.enumerator(eventEnumIndex).valueToKey(event->type()) << " filter =" << filter_val;
		qDebug() << "index data : " << index.model()->data(index, Qt::DisplayRole).toString();
		qDebug() << "model data : " << model->data(index, Qt::DisplayRole).toString();

	}

	return filter_val;
}



























































