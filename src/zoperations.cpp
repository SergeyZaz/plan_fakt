#include <QInputDialog>
#include <QMessageBox>
#include "zoperations.h"
#include "zoperationsform.h"

ZOperations::ZOperations()
{
	QStringList items;
	items << tr("���������� ������") << tr("���������� ������");
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

	if(txt == "���������� ������")
	{
		strTbl = "projects";
		strAtt = "project";
	
		loadItemsToList(items, strTbl);

		text = QInputDialog::getItem(this, tr("��������� �������"),
			tr("�������:"), items, 0, false, &ok);
	}
	else
	{
		loadItemsToList(items, strTbl);

		text = QInputDialog::getItem(this, tr("��������� ������"),
			tr("������:"), items, 0, false, &ok);
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
		tId = m_tbl->getModel()->data(m_tbl->getModel()->index(indx.row(), 0)).toInt(); // id ������ ���� 0-� ��������!!!
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
		QMessageBox::critical(this, tr("������"), query.lastError().text());
		return 0;
	}
	return 1;
}

void ZOperations::initDB(QSqlDatabase &m_DB, const QString &m_TblName)
{
	m_tbl->setDatabase(m_DB);
/*
  id serial NOT NULL,
  date text, -- ���� ��������
  type integer, -- ���: 0-�����������/1-�������/2-�����������
  comment text, -- �����������
  ur_person integer, -- ��.����
  partner integer, -- ����������
  section integer, -- ������
  project integer, -- ������
  val double precision, -- �����
*/
	QList<int> hideColumns;
	QStringList headers;
	QList<int> cRem;
	
	hideColumns << 0;
	headers <<  tr("id") << tr("���� ��������") << tr("���")  << tr("�����������") << tr("��.����") << tr("����������") << tr("������") << tr("������") << tr("�����");

	m_tbl->setTable(m_TblName, headers, cRem);
	m_tbl->setCustomEditor(new ZOperationsForm);
	
	QMap<int, QString> *pMap = new QMap<int, QString>;
	pMap->insert(0, tr("�����������"));
	pMap->insert(1, tr("�������"));
	pMap->insert(2, tr("�����������"));
	m_tbl->setRelation(2, pMap);
	
	m_tbl->setRelation(4, "ur_persons", "id", "name");
	m_tbl->setRelation(5, "partners", "id", "name");
	m_tbl->setRelation(6, "sections", "id", "name");
	m_tbl->setRelation(7, "projects", "id", "name");
	
	m_tbl->init(hideColumns);
	m_tbl->moveSection(3, 8);
}

