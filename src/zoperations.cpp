#include <QInputDialog>
#include <QMessageBox>
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include "zoperations.h"
#include "zoperationsform.h"

ZOperations::ZOperations()
{
	QStringList items;
	items << tr("Установить проект") << tr("Установить статью") << tr("Установить тип") << tr("Выгрузить в файл");
	setContextMenuForTbl(items);

	connect(m_tbl, SIGNAL(needUpdateVal(int)), this,SIGNAL(needUpdateVal(int)));
}

void ZOperations::loadItemsToList(QStringList &l_Text, QList<int> &l_id, const QString &tableName)
{
	QSqlQuery query;
	QString strQuery;

	if(tableName == "sections")
		strQuery = QString("SELECT name, id, parent FROM %1 ORDER BY name").arg(tableName);
	else
		strQuery = QString("SELECT name, id FROM %1 ORDER BY name").arg(tableName);

	if (query.exec(strQuery))
	{
		while (query.next()) 
		{
			strQuery = query.value(0).toString().simplified();
			if(tableName == "sections")	//0-операционный поток, 1-инвестиционный поток, 2-финансовый поток
			{
				switch(query.value(2).toInt())
				{
				case 1:
					strQuery += tr(" (инвестиционный поток)");
					break;
				case 2:
					strQuery += tr(" (финансовый поток)");
					break;
				default:
					strQuery += tr(" (операционный поток)");
					break;
				}
			}
			l_Text << strQuery;
			l_id << query.value(1).toInt();
		}
	}
}

void ZOperations::execCustomAction(const QString &txt)
{
	QString strAtt;
	QString text;
	bool ok;
	QStringList items;
	QList<int> itemIds;
	int id;

	if(txt == "Установить проект")
	{
		strAtt = "project";
	
		loadItemsToList(items, itemIds, "projects");

		text = QInputDialog::getItem(this, tr("Установка проекта"),
			tr("Проекты:"), items, 0, false, &ok);
	}
	else if(txt == "Установить статью")
	{
		strAtt = "section";

		loadItemsToList(items, itemIds, "sections");

		text = QInputDialog::getItem(this, tr("Установка статьи"),
			tr("Статьи:"), items, 0, false, &ok);
	}
	else if(txt == "Установить тип")
	{
		strAtt = "type";

		items << tr("Поступление") << tr("Выплата") << tr("Перемещение");
		itemIds << 0 << 1 << 2;

		text = QInputDialog::getItem(this, tr("Установка типа"),
			tr("Типы:"), items, 0, false, &ok);
	}
	else if(txt == "Выгрузить в файл")
	{
		exportSelectedItems();
		return;
	}

	if (!ok || text.isEmpty())
		return;

	id = items.indexOf(text);
	if (id >= itemIds.size())
		return;
	id = itemIds.at(id);

	if(updateSelectedItems(id, strAtt))
		reload();
}

#ifdef SAVETOXML
#include <QAxObject>
void val2Cell(QAxObject* StatSheet, int row, int col, QString &s)
{
	if(!StatSheet)
		return;
	// получение указателя на ячейку [row][col] ((!)нумерация с единицы)
	QAxObject *cell = StatSheet->querySubObject("Cells(Int,Int)", row, col);
	// вставка значения переменной data (любой тип, приводимый к QVariant) в полученную ячейку
	if(!cell)
		return;
	s = s.replace(QChar::Nbsp, "");
	bool ok;
	double v = s.toDouble(&ok);
	if(ok)
		cell->dynamicCall("SetValue(const QVariant&)", v);
	else						
		cell->dynamicCall("SetValue(const QVariant&)", s);
    delete cell;
}
#endif //SAVETOXML

int ZOperations::exportSelectedItems()
{
	QString fileName = QFileDialog::getSaveFileName(this);
	if (fileName.isEmpty()) 
		return 0;

#ifdef SAVETOXML
	if(!fileName.endsWith(".xlsx"))
		fileName += ".xlsx";
#else
	if(!fileName.endsWith(".csv"))
		fileName += ".csv";
#endif //SAVETOXML

#ifdef SAVETOXML
	// получаем указатель на Excel
    QAxObject *mExcel = new QAxObject("Excel.Application",this);
	//делаем его видимым
	//mExcel->dynamicCall( "SetVisible(bool)", TRUE ); 
    // получаем указатель на книги
    QAxObject *workbooks = mExcel->querySubObject("Workbooks");
    // добавляем книгу
	workbooks->dynamicCall("Add");
    // получаем указатель на текущую книгу
	QAxObject *workbook = mExcel->querySubObject("ActiveWorkBook");
	if(!workbook)
		return 0;
    // получаем указатель на листы
    QAxObject *mSheets = workbook->querySubObject( "Sheets" );
    // указываем, какой лист выбрать
    QAxObject *StatSheet = mSheets->querySubObject( "Item(const QVariant&)", QVariant("Лист1") );
	if(!StatSheet)
		return 0;
	int r = 1;
#else
	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		QMessageBox::critical(NULL, QString("Ошибка"), QString("Ошибка создания файла!"));
		return 0;
	}

	QTextStream out(&file);	 
#endif //SAVETOXML

	QTableView *tbl = m_tbl->getTable();
	QModelIndexList listIndxs = tbl->selectionModel()->selectedRows();

	int row, i, columns = m_tbl->getModel()->columnCount();
	for(i=0; i<columns; i++)
	{
		if(!m_tbl->getTable()->isColumnHidden(i))
#ifdef SAVETOXML
			val2Cell(StatSheet, r, i, m_tbl->getModel()->headerData(i, Qt::Horizontal).toString());
#else
			out << m_tbl->getModel()->headerData(i, Qt::Horizontal).toString() << ";";
#endif //SAVETOXML
	}
#ifdef SAVETOXML
	r++;
#else
	out << "\n";
#endif //SAVETOXML

	foreach(QModelIndex index, listIndxs)
	{
		row = m_tbl->getSortModel()->mapToSource(index).row();
		for(i=0; i<columns; i++)
		{
			if(!m_tbl->getTable()->isColumnHidden(i))
			{
				QString text = m_tbl->getModel()->data(m_tbl->getModel()->index(row, i)).toString();
#ifdef SAVETOXML
				val2Cell(StatSheet, r, i, text);
#else
				if(i==8)// Сумма
					out << text.replace(".", ",") << ";";
				else
					out << text << ";";
#endif //SAVETOXML
			}
		}
#ifdef SAVETOXML
		r++;
#else
		out << "\n";
#endif //SAVETOXML
	}

#ifdef SAVETOXML
	QString tmp_s = fileName;
	//правим слеши, насколько необходимо не помню
	tmp_s.replace("/","\\");
	//говорим excel что всякие служебные сообщения выводить не надо
	mExcel->setProperty("DisplayAlerts", 0);
	//сохраняем наш файл под сгенерированным именем в каталог с программой
	workbook->dynamicCall("SaveAs (const QString&)", tmp_s);

	mExcel->setProperty("DisplayAlerts", 1);

	//зачем-то надо, канонично сразу не закомменитировал, теперь не помн
	workbook->dynamicCall("Close(Boolean)", false);

    delete StatSheet;
    delete mSheets;
    delete workbook;
    delete workbooks;
	delete mExcel;
#endif //SAVETOXML

	QMessageBox::information(this, QString("Выполнено"), QString("Выбранные операции выгружены в файл!"));
	return 1;
}

int ZOperations::updateSelectedItems(int setId, const QString &attName)
{
	QSqlQuery query;
	QTableView *tbl = m_tbl->getTable();
	QModelIndexList listIndxs = tbl->selectionModel()->selectedRows();
	QList<int> ids;
	int tId;
	foreach(QModelIndex index, listIndxs)
	{
		QModelIndex indx = m_tbl->getSortModel()->mapToSource(index);
		tId = m_tbl->getModel()->data(m_tbl->getModel()->index(indx.row(), 0)).toInt(); // id должен быть 0-м столюцом!!!
		if(tId!=0 && !ids.contains(tId))
			ids.push_back(tId);
	}
	
	if(!ids.size())
		return 0;

	QString strQuery = QString("UPDATE operations SET %1=%2 WHERE id IN(")
		.arg(attName)
		.arg(setId);
	foreach(int id, ids)
	{
		strQuery += QString::number(id) + ",";
	}
	strQuery.chop(1);
	strQuery += ")";
	
	if (!query.exec(strQuery))
	{
		QMessageBox::critical(this, tr("Ошибка"), query.lastError().text());
		return 0;
	}
	return 1;
}

void ZOperations::initDB(QSqlDatabase &m_DB, const QString &m_TblName)
{
	m_tbl->setDatabase(m_DB);
/*
  id serial NOT NULL,
  date text, -- Дата операции
  type integer, -- Тип: 0-Поступление/1-Выплата/2-Перемещение
  comment text, -- Комментарий
  ur_person integer, -- Юр.лицо
  partner integer, -- контрагент
  section integer, -- статья
  project integer, -- проект
  val double precision, -- Сумма
  account integer, -- Счет
  num integer, -- Номер операции
*/
	QList<int> hideColumns;
	QStringList headers;
	QList<int> cRem;
	
	hideColumns << 0;
#ifndef _DEBUG
	hideColumns << 10;
#endif

	headers <<  tr("id") << tr("Дата операции") << tr("Тип")  << tr("Комментарий") << tr("Юр.лицо") << tr("Контрагент") << tr("Статья") << tr("Проект") << tr("Сумма") << tr("Счёт") << tr("Номер");

	m_tbl->setTable(m_TblName, headers, cRem);
	m_tbl->setCustomEditor(new ZOperationsForm);
	
	QMap<int, QString> *pMap = new QMap<int, QString>;
	pMap->insert(0, tr("Поступление"));
	pMap->insert(1, tr("Выплата"));
	pMap->insert(2, tr("Перемещение"));
	m_tbl->setRelation(2, pMap);
	
	m_tbl->setRelation(4, "ur_persons", "id", "name");
	m_tbl->setRelation(5, "partners", "id", "name");
	m_tbl->setRelation(6, "sections", "id", "name");
	m_tbl->setRelation(7, "projects", "id", "name");
	m_tbl->setRelation(9, "accounts", "id", "name");
	
	m_tbl->init(hideColumns);
	m_tbl->moveSection(3, 8);
	m_tbl->moveSection(9, 4);
}

