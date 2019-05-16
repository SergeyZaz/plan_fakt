#include <QInputDialog>
#include <QMessageBox>
#include "zoperations.h"
#include "zoperationsform.h"

ZOperations::ZOperations()
{
	QStringList items;
	items << tr("���������� ������") << tr("���������� ������") << tr("���������� ���");
	setContextMenuForTbl(items);
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
			if(tableName == "sections")	//0-������������ �����, 1-�������������� �����, 2-���������� �����
			{
				switch(query.value(2).toInt())
				{
				case 1:
					strQuery += tr(" (�������������� �����)");
					break;
				case 2:
					strQuery += tr(" (���������� �����)");
					break;
				default:
					strQuery += tr(" (������������ �����)");
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

	if(txt == "���������� ������")
	{
		strAtt = "project";
	
		loadItemsToList(items, itemIds, "projects");

		text = QInputDialog::getItem(this, tr("��������� �������"),
			tr("�������:"), items, 0, false, &ok);
	}
	else if(txt == "���������� ������")
	{
		strAtt = "section";

		loadItemsToList(items, itemIds, "sections");

		text = QInputDialog::getItem(this, tr("��������� ������"),
			tr("������:"), items, 0, false, &ok);
	}
	else if(txt == "���������� ���")
	{
		strAtt = "type";

		items << tr("�����������") << tr("�������") << tr("�����������");
		itemIds << 0 << 1 << 2;

		text = QInputDialog::getItem(this, tr("��������� ����"),
			tr("����:"), items, 0, false, &ok);
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
		tId = m_tbl->getModel()->data(m_tbl->getModel()->index(indx.row(), 0)).toInt(); // id ������ ���� 0-� ��������!!!
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
  account integer, -- ����
*/
	QList<int> hideColumns;
	QStringList headers;
	QList<int> cRem;
	
	hideColumns << 0;
	headers <<  tr("id") << tr("���� ��������") << tr("���")  << tr("�����������") << tr("��.����") << tr("����������") << tr("������") << tr("������") << tr("�����") << tr("����");

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
	m_tbl->setRelation(9, "accounts", "id", "name");
	
	m_tbl->init(hideColumns);
	m_tbl->moveSection(3, 8);
	m_tbl->moveSection(9, 4);
}

