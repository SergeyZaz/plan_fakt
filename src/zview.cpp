#include <QCompleter>
#include <QDateTime>
#include <QDateTimeEdit>
#include <QHelpEvent>
#include <QMessageBox>
#include <QPainter>
#include <QPrintDialog>
#include <QPrinter>
#include <QHeaderView>
#include "zview.h"
#include "ztoolwidget.h"

#define HIGHLIGHTCOLOR Qt::cyan
#define PLUS_COLOR	QColor(85, 255, 127)
#define MINUS_COLOR	QColor(255, 170, 127)


QPixmap *ZToolWidget::pixmap = NULL;
ZToolWidget *ZToolWidget::pZToolWidget = NULL;

ZView::ZView(QWidget *parent, Qt::WFlags flags)
	: QWidget(parent, flags)
{
	init();
}

ZView::ZView(QSqlDatabase &DB, QWidget *parent, Qt::WFlags flags)
	: QWidget(parent, flags)
{
	init();
	setDatabase(DB);
}
	
void ZView::init()
{
	model = NULL;
	ui.setupUi(this);
	//setAttribute(Qt::WA_DeleteOnClose);

	connect(ui.txtFilter, SIGNAL(textChanged(const QString &)), this, SLOT(changeFilter(const QString &)));
	connect(ui.cboFilter, SIGNAL(currentIndexChanged(int)), this, SLOT(changeFilter(int)));
	connect(ui.cmdAdd, SIGNAL(clicked()), this, SLOT(add()));
	connect(ui.cmdDel, SIGNAL(clicked()), this, SLOT(del()));
	connect(ui.cmdEdit, SIGNAL(clicked()), this, SLOT(edit()));
	connect(ui.tbl, SIGNAL(clicked ( const QModelIndex &)), this, SLOT(clickedTbl ( const QModelIndex &)));
	connect(ui.tbl, SIGNAL(doubleClicked ( const QModelIndex &)), this, SLOT(doubleClickedTbl ( const QModelIndex &)));
	connect(ui.cmdPrint, SIGNAL(clicked()), this, SLOT(PrintCards()));
	connect(ui.cmdReload, SIGNAL(clicked()), this, SLOT(reload()));
	
	
	sortModel.setDynamicSortFilter(true); 
	sortModel.setFilterKeyColumn(1); 

	ui.tbl->horizontalHeader()->setMovable(true);
	ui.tbl->verticalHeader()->hide();
	ui.tbl->installEventFilter(this);

	ui.cmdPrint->setVisible(false);
	
	int rc;
	pEditForm = new ZEditBaseForm(this);
	rc = connect(pEditForm, SIGNAL(accepted()), this, SLOT(applyEditor()));
	rc = connect(pEditForm, SIGNAL(errorQuery(const QDateTime&, long, const QString&)), this, SIGNAL(errorQuery(const QDateTime&, long, const QString&)));	
}

ZView::~ZView()
{
	if(pEditForm)
	{
		delete pEditForm;
		pEditForm = NULL;
	}
	if(model)
		delete model;
}
	
void ZView::setDatabase(QSqlDatabase &DB)
{
	m_DB = DB;
}
	
void ZView::setColorHighligthIfColumnContain(int col, QList<int> *plist)
{
	ZTableModel *pModel = dynamic_cast<ZTableModel*>(model);
	if(!pModel || !plist)
		return;
	pModel->pHighlightItems = plist;
	pModel->m_HighlightColumn = col;
}

void ZView::setRelation(int column, const QString &tbl, const QString &attId, const QString &attValue)
{
	ZTableModel *pModel = dynamic_cast<ZTableModel*>(model);
	if(pModel) 
		pModel->setRelation(column, QSqlRelation(tbl, attId, attValue));
}
	
void ZView::setRelation(int column, QMap<int, QString> *relMap)
{
	ZTableModel *pModel = dynamic_cast<ZTableModel*>(model);
	if(pModel) 
		pModel->relMaps.insert(column, relMap);
}

int ZView::setTable(const QString &tbl, QStringList &headers, QList<int> &removeColumns)
{
	if(pEditForm)
	{
		if(windowTitle() != "ZView")
		{
			QString text = QString("%1 \"%2\"").arg(pEditForm->windowTitle()).arg(windowTitle());
			pEditForm->setWindowTitle(text);
		}
	}

	mTable = tbl;

	ZTableModel *pModel = new ZTableModel(this, m_DB);
	pModel->setTable(tbl);
	pModel->setEditStrategy(QSqlTableModel::OnFieldChange);

	sortModel.setSourceModel(pModel); 
	model = pModel;

	int indx;

	for(indx=0; indx<headers.size(); indx++)
	{
		QString text = headers.at(indx);
		model->setHeaderData(indx, Qt::Horizontal, text);
		ui.cboFilter->addItem(text, indx);
	}

	qSort(removeColumns.begin(), removeColumns.end(), qGreater<int>());
	foreach(indx, removeColumns)
	{
		model->removeColumn(indx);
	}
	return 1;
}

int ZView::setQuery(const QString &query, QStringList &headers, bool fRemoveOldModel)
{
	if(model && fRemoveOldModel)
	{
		delete model;
		model = NULL;
	}

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	if(!model)
		model = new QSqlQueryModel();

	model->setQuery(query, m_DB);
	if (model->lastError().isValid())
	{
		QApplication::restoreOverrideCursor();
		emit errorQuery(QDateTime::currentDateTime(), model->lastError().number(), model->lastError().text());
		return 0;
	}

	sortModel.setSourceModel(model); 

	int indx;
	
	ui.cboFilter->clear();

	for(indx=0; indx<headers.size(); indx++)
	{
		QString text = headers.at(indx);
		model->setHeaderData(indx, Qt::Horizontal, text);
		ui.cboFilter->addItem(text, indx);
	}
	
	QApplication::restoreOverrideCursor();
	return 1;
}

int ZView::init(QList<int> &hideColumns, int sortCol)
{
	if(!model)
		return 0;

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	ZTableModel *pModel = dynamic_cast<ZTableModel*>(model);
	if(pModel)
	{
		pModel->select();
//		sortModel.setSourceModel(model); 

		if (model->lastError().isValid())
		{
			QApplication::restoreOverrideCursor();
			emit errorQuery(QDateTime::currentDateTime(), model->lastError().number(), model->lastError().text());
			return 0;
		}
	}

	//ui.tbl->horizontalHeader()->setMovable(true);
	ui.tbl->horizontalHeader()->setResizeMode(QHeaderView::Interactive);//ResizeToContents);//QHeaderView::Interactive);
	ui.tbl->setModel(&sortModel);

	int indx;
	qSort(hideColumns.begin(), hideColumns.end(), qGreater<int>());
	foreach(indx, hideColumns)
	{
		ui.tbl->setColumnHidden(indx, true);
		ui.cboFilter->removeItem(indx);
	}

	ui.tbl->horizontalHeader()->setSortIndicator(sortCol, Qt::AscendingOrder); 
	
	QApplication::restoreOverrideCursor();
	return 1;
}

void ZView::moveSection(int from, int to)
{
	ui.tbl->horizontalHeader()->moveSection(from, to);
}
	
void ZView::update()
{
	if(!model)
		return;

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	
	ZTableModel *pModel = dynamic_cast<ZTableModel*>(model);
	if(pModel)
	{
		pModel->select();

		if (model->lastError().isValid())
		{
			QApplication::restoreOverrideCursor();
			emit errorQuery(QDateTime::currentDateTime(), model->lastError().number(), model->lastError().text());
			return;
		}
	}
	else
	{
		model->setQuery(model->query().lastQuery(), m_DB);
	}

	QApplication::restoreOverrideCursor();
}

void ZView::updateAndShow(bool fMaximized)
{
	update();

	//делаем окно активным
	if (isMinimized())
		setWindowState(windowState() &  ~Qt::WindowMinimized | Qt::WindowActive);
	else
		show();

	activateWindow();

	raise();
	
	if(fMaximized)
		showMaximized();
	else
		show();
}

void ZView::reload()
{
	update();
}

void ZView::add()
{
	//ZTableModel *pModel = dynamic_cast<ZTableModel*>(model);
	//if(pModel) 
	//	pModel->insertRows(0, 1); 
	openEditor();
}

void ZView::edit()
{
	if(!model)
		return;
	QModelIndexList listIndxs = ui.tbl->selectionModel()->selectedRows();
	if(listIndxs.size()==0)
	{
		QMessageBox::warning( this, tr("Внимание"), tr("Выберите элемент из списка и повторите попытку"));
		return;
	}
	QModelIndex indx = sortModel.mapToSource(listIndxs.first());
	int id = model->data(model->index(indx.row(), 0)).toInt();

	openEditor(id);
}

int ZView::openEditor(int id)
{
	if(!model)
		return 0;

	printf("edit id = %d\n", id);

	if(!pEditForm || id == 0 || id == -1)
		return 0;

	if(!pEditForm->init(m_DB, mTable, id))
		return 0;
	
	pEditForm->show();
	return 1;
}

void ZView::applyEditor()
{
	if(m_DB.isOpen())
	{
		QModelIndex indx = ui.tbl->currentIndex();

		update();

		ui.tbl->setCurrentIndex(indx);
		ui.tbl->scrollTo(indx);
	}
}
	
void ZView::setCustomEditor(ZEditAbstractForm *pD)
{
	if(pEditForm)
		delete pEditForm;
	pEditForm = pD;
	int rc;
	rc = connect(pEditForm, SIGNAL(accepted()), this, SLOT(applyEditor()));
	rc = connect(pD, SIGNAL(errorQuery(const QDateTime &, long , const QString &)), this, SIGNAL(errorQuery(const QDateTime &, long , const QString &)));
}

void ZView::del()
{
	if(!model)
		return;

	QModelIndexList listIndxs = ui.tbl->selectionModel()->selectedRows();
	QList<int> ids;
	int tId;
	foreach(QModelIndex index, listIndxs)
	{
		QModelIndex indx = sortModel.mapToSource(index);
		tId = model->data(model->index(indx.row(), 0)).toInt(); // id должен быть 0-м столюцом!!!
		if(tId!=0 && !ids.contains(tId))
			ids.push_back(tId);
	}
	
	if(!ids.size())
		return;
	if ( QMessageBox::question( this, tr("Внимание"), tr("Вы действительно хотите удалить выделенные элементы?"), tr("Да"), tr("Нет"), QString::null, 0, 1 ) != 0)
		return;

	QSqlQuery m_Query(m_DB);
	QString s_Query = tr("DELETE FROM %1 WHERE id IN (").arg(mTable);

	for(int i=0;i<ids.size();i++)
	{
		printf("remove id = %d\n", ids[i]);

		if(i!=0)
			s_Query += ",";

		s_Query += QString::number(ids[i]);
	}

	s_Query += ")";
			
	if(!m_Query.exec(s_Query))
	{
		emit errorQuery(QDateTime::currentDateTime(), m_Query.lastError().number(), m_Query.lastError().text());
		return;
	}

	emit needUpdate();

	update();
}

void ZView::changeFilter(int indx)
{
	if(!model)
		return;
	sortModel.setFilterKeyColumn(ui.cboFilter->itemData(indx).toInt()); 
	changeFilter(ui.txtFilter->text());
}

void ZView::changeFilter(const QString &text)
{
	if(!model)
		return;
	
	ui.txtFilter->blockSignals(true);
	ui.txtFilter->setText(text);
	ui.txtFilter->blockSignals(false);

    QRegExp regExp(text, Qt::CaseInsensitive); 
	sortModel.setFilterRegExp(regExp);
}

bool ZView::eventFilter(QObject *obj, QEvent *event)
{
	if (event->type() == QEvent::ToolTip)
	{
		QPixmap pixmap;
		QHelpEvent *pHelpEvent = dynamic_cast<QHelpEvent*>(event);
		bool rc = false;
		if(pHelpEvent)
			rc = ZToolWidget::Show(pHelpEvent->globalPos());
		return rc;
	} 
	return QObject::eventFilter(obj, event);
}

void ZView::clickedTbl(const QModelIndex &index)
{
	if(!model)
		return;
	QModelIndex indx = sortModel.mapToSource(index);
	int id = model->data(model->index(indx.row(), 0)).toInt();

	ui.cmdDel->setEnabled(id>0); // удалить нельзя
	ui.cmdEdit->setEnabled(id>0); // редактировать нельзя

	emit setCurrentElem(QEvent::MouseButtonRelease, id);
}


void ZView::doubleClickedTbl(const QModelIndex &index)
{
	if(!model)
		return;
	QModelIndex indx = sortModel.mapToSource(index);
	int id = model->data(model->index(indx.row(), 0)).toInt();

	emit setCurrentElem(QEvent::MouseButtonDblClick, id);

	openEditor(id);
}

	
void ZView::setReadOnly(bool fEdit)
{
	ui.cmdAdd->setVisible(!fEdit);
	ui.cmdDel->setVisible(!fEdit);
	ui.cmdEdit->setVisible(!fEdit);
	ZTableModel *pModel = dynamic_cast<ZTableModel*>(model);
	if(pModel) 
		pModel->fEdit = !fEdit;
}
	
QItemSelectionModel *ZView::selectionModel()
{
	return ui.tbl->selectionModel();
}

void ZView::setPhotoColumn(int col) 
{
	ZTableModel *pModel = dynamic_cast<ZTableModel*>(model);
	if(pModel) 
		pModel->m_PhotoColumn = col;
}

void ZView::setHexColumn(int col)
{ 
	ZTableModel *pModel = dynamic_cast<ZTableModel*>(model);
	if(pModel) 
		pModel->m_HexColumn = col;
}

void ZView::setFilter(const QString & filter)
{ 
	ZTableModel *pModel = dynamic_cast<ZTableModel*>(model);
	if(pModel) 
		pModel->setFilter(filter);
}

	
void ZView::setVisiblePrint(bool fTrue)
{
	ui.cmdPrint->setVisible(fTrue);
}

#define PRINT_WIDTH			200
#define PRINT_HEIGHT		300
#define PRINT_OTSTUP		5
#define PRINT_FIELD_HEIGHT	20

int PrintCard(int row, QSqlQueryModel *pModel, int &number, QPrinter &printer, QPainter &painter)
{
		QFont fnt = painter.font();

		fnt.setBold(true);
		painter.setFont(fnt);

		QRect r = printer.pageRect();
		printf("r.width()=%d, r.height()=%d\n", r.width(), r.height());

		int numInRow = r.width() / PRINT_WIDTH;
		//int numInCol = r.height() / PRINT_HEIGHT;
		int y = number / numInRow,
			x = number - y*numInRow;

		if((y+1)*PRINT_HEIGHT > r.height())
		{
			number = 0;
			x = y = 0;
			if (!printer.newPage()) 
				return 0;
		}

		printf("x=%d, y=%d\n", x, y);

		x *= PRINT_WIDTH;
		y *= PRINT_HEIGHT;

		painter.drawRect( x+PRINT_OTSTUP, y+PRINT_OTSTUP, PRINT_WIDTH, PRINT_HEIGHT );

		QRect rect;
		int curPos = y+PRINT_FIELD_HEIGHT;
		
		// Воинская часть
		rect = QRect(x+2*PRINT_OTSTUP, curPos, PRINT_WIDTH-PRINT_OTSTUP, PRINT_FIELD_HEIGHT);
		painter.drawText(rect, Qt::AlignCenter, pModel->data(pModel->index(row, 7)).toString() );
		curPos += PRINT_FIELD_HEIGHT;

		fnt.setBold(false);
		painter.setFont(fnt);

		QSize imageSize = QSize( PRINT_WIDTH - 2*PRINT_OTSTUP, 140 );
		QImage image;
		//image.loadFromData(pModel->data(pModel->index(row, 11), Qt::UserRole+1).toByteArray());
		if(image.loadFromData(pModel->data(pModel->index(row, 11)).toByteArray()))
		{
			QImage scaledImage = image.scaled( imageSize, Qt::KeepAspectRatio, Qt::FastTransformation );
			// Фотография
			painter.drawImage(x+(PRINT_WIDTH - 2*PRINT_OTSTUP - scaledImage.width()) / 2.0 + 10, curPos, scaledImage);
		}
		curPos += 140;

		// Подразделение
		rect = QRect(x+2*PRINT_OTSTUP, curPos, PRINT_WIDTH - 2*PRINT_OTSTUP, PRINT_FIELD_HEIGHT);
		painter.drawText(rect, Qt::AlignCenter, pModel->data(pModel->index(row, 5)).toString() );
		curPos += PRINT_FIELD_HEIGHT;

		fnt.setBold(true);
		painter.setFont(fnt);

		// Звание
		rect = QRect(x+2*PRINT_OTSTUP, curPos, PRINT_WIDTH - 2*PRINT_OTSTUP, PRINT_FIELD_HEIGHT);
		painter.drawText(rect, Qt::AlignCenter, pModel->data(pModel->index(row, 4)).toString() );
		curPos += PRINT_FIELD_HEIGHT;

		int oldSize = fnt.pointSize();
		if(oldSize!=-1)
		{
			painter.setFont(fnt);
			fnt.setPointSize(oldSize+5);
		}

		// Фамилия
		rect = QRect(x+2*PRINT_OTSTUP, curPos, PRINT_WIDTH - 2*PRINT_OTSTUP, PRINT_FIELD_HEIGHT);
		painter.drawText(rect, Qt::AlignCenter, pModel->data(pModel->index(row, 1)).toString() );
		curPos += PRINT_FIELD_HEIGHT;

		if(oldSize!=-1)
		{
			fnt.setPointSize(oldSize);
			painter.setFont(fnt);
		}

		// Имя
		rect = QRect(x+2*PRINT_OTSTUP, curPos, PRINT_WIDTH - 2*PRINT_OTSTUP, PRINT_FIELD_HEIGHT);
		painter.drawText(rect, Qt::AlignCenter, pModel->data(pModel->index(row, 2)).toString() );
		curPos += PRINT_FIELD_HEIGHT;

		// Отчество
		rect = QRect(x+2*PRINT_OTSTUP, curPos, PRINT_WIDTH - 2*PRINT_OTSTUP, PRINT_FIELD_HEIGHT);
		painter.drawText(rect, Qt::AlignCenter, pModel->data(pModel->index(row, 3)).toString() );
		curPos += PRINT_FIELD_HEIGHT;

		fnt.setBold(false);
		painter.setFont(fnt);

		return 1;
}

void ZView::PrintCards()
{
	QPainter painter;
	QPrinter printer;

	QModelIndexList listIndxs = ui.tbl->selectionModel()->selectedRows();
	if(listIndxs.size()==0)
		return;

	QPrintDialog printDialog( &printer );
	// Размер страницы в точках
	printer.setPaperSize( QPrinter::A4 );

	if ( printDialog.exec() == QDialog::Accepted )
	{
		painter.begin( &printer );

		int i=0;
		foreach(QModelIndex index, listIndxs)
		{
			QModelIndex indx = sortModel.mapToSource(index);
			if(!PrintCard(indx.row(), model, i, printer, painter))
				break;
			i++;
		}

		painter.end();
	}
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
ZTableModel::ZTableModel(QObject *parent, QSqlDatabase db): QSqlRelationalTableModel(parent, db)
{
	m_PhotoColumn = -1;
	m_HexColumn = -1;
	m_HighlightColumn = -1;
	pHighlightItems = NULL;
	fEdit = false;
}
	
ZTableModel::~ZTableModel()
{
	QMapIterator<int , QMap<int, QString>* > iT(relMaps);
	while (iT.hasNext()) 
	{
		iT.next();
		delete iT.value();
	}	
	relMaps.clear();
}

QVariant ZTableModel::data(const QModelIndex & index, int role) const
{
	QVariant v;
	int col = index.column();
	int row = index.row();

	if(col == m_PhotoColumn)
	{
		v = QSqlTableModel::data(index, Qt::DisplayRole);

		if(v.toByteArray().size())
		{
			switch(role)
			{
			case Qt::UserRole+1:
				return v;
			case Qt::BackgroundColorRole:
				return Qt::green;
			case Qt::DisplayRole:
				return QString("есть");
			case Qt::ToolTipRole:
				{
					QPixmap pixmap;
					if(pixmap.loadFromData(v.toByteArray()))
					{
						ZToolWidget::pixmap = new QPixmap(pixmap);
					}
					return QVariant();
				}
			default:
				break;
			}
		}
	}

	if(pHighlightItems && m_HighlightColumn!=-1 && role==Qt::BackgroundColorRole)
	{
		QModelIndex t_indx = this->index(row, 0);
		v = QSqlTableModel::data(t_indx, Qt::DisplayRole);
		if(pHighlightItems->contains(v.toInt()))
			return HIGHLIGHTCOLOR;
	}
	
	if(role==Qt::BackgroundColorRole)
	{
		v = QSqlTableModel::data(index, Qt::DisplayRole);
		if(v.type() == QVariant::Double)
		{
			if(QSqlTableModel::data(this->index(row, 2), Qt::DisplayRole).toInt() == 1)//Тип: 0-Поступление/1-Выплата/2-Перемещение
			//if(v.toDouble() < 0)
				return MINUS_COLOR;
			else
				return PLUS_COLOR;
		}
	}

	v = QSqlTableModel::data(index, role);
	
	if(v.type() == QVariant::Double)
		return QString::number(v.toDouble(), 'f', 2);
	
	switch(role)
	{
	case Qt::DisplayRole:
		{
			if(col == m_HexColumn)
			{
				return QString::number(v.toInt(), 16).toUpper();
			}

			const QMap<int, QString> *pRelMap = relMaps.value(col, NULL);
			if(pRelMap)
				return pRelMap->value(v.toInt());
		}
		break;
	default:
		break;
	}

	return v;
}

Qt::ItemFlags ZTableModel::flags(const QModelIndex &) const
{
	Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
	if(fEdit)
		flags |= Qt::ItemIsEditable;
	return flags;
}

/////////////////////////////////////////////////////////////////////////////////////

bool ZSortFilterProxyModel::lessThan(const QModelIndex &left,
                                       const QModelIndex &right) const
 {
     QVariant leftData = sourceModel()->data(left);
     QVariant rightData = sourceModel()->data(right);
	 bool ok;
	 double d = leftData.toDouble(&ok);
	 if ( ok )
	 {
		 return d < rightData.toDouble();
     }
	 return QSortFilterProxyModel::lessThan(left, right);
 }
 