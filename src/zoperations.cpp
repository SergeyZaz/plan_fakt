#include <QInputDialog>
#include <QMessageBox>
#include "zoperations.h"
#include "zoperationsform.h"

ZOperations::ZOperations()
{
	QStringList items;
	items << tr("Установить проект") << tr("Установить статью");
	setContextMenuForTbl(items);
}

void ZOperations::loadItemsToList(QStringList &list, const QString &tableName)
{
	QSqlQuery query;
	if (query.exec(QString("SELECT name FROM %1 ORDER BY name").arg(tableName)))
	{
		while (query.next()) 
		{
			list << query.value(0).toString().simplified();
		}
	}
}

void ZOperations::execCustomAction(const QString &txt)
{
	QString strTbl = "sections";
	QString strAtt = "section";
	QString text;
	bool ok;
	QStringList items;

	if(txt == "Установить проект")
	{
		strTbl = "projects";
		strAtt = "project";
	
		loadItemsToList(items, strTbl);

		text = QInputDialog::getItem(this, tr("Установка проекта"),
			tr("Проекты:"), items, 0, false, &ok);
	}
	else
	{
		loadItemsToList(items, strTbl);

		text = QInputDialog::getItem(this, tr("Установка статьи"),
			tr("Статьи:"), items, 0, false, &ok);
	}

	if (!ok || text.isEmpty())
		return;
	if(updateSelectedItems(text, strTbl, strAtt))
		reload();
}

int ZOperations::updateSelectedItems(const QString &elemName, const QString &tableName, const QString &attName)
{
	QSqlQuery query;
	int setId = 0;
	if (query.exec(QString("SELECT id FROM %1 WHERE name='%2'").arg(tableName).arg(elemName)))
	{
		while (query.next()) 
		{
			setId = query.value(0).toInt();
			break;
		}
	}
	else
		return 0;

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

	QString strQuery = QString("UPDATE operations SET %1=%2 WHERE id IN(").
		arg(attName).
		arg(setId);
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
*/
	QList<int> hideColumns;
	QStringList headers;
	QList<int> cRem;
	
	hideColumns << 0;
	headers <<  tr("id") << tr("Дата операции") << tr("Тип")  << tr("Комментарий") << tr("Юр.лицо") << tr("Контрагент") << tr("Статья") << tr("Проект") << tr("Сумма");

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
	
	m_tbl->init(hideColumns);
	m_tbl->moveSection(3, 8);
}

