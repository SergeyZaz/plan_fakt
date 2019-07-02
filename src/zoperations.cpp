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
	items << tr("���������� ������") << tr("���������� ������") << tr("���������� ���") << tr("��������� � ����");
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
	else if(txt == "��������� � ����")
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
	// ��������� ��������� �� ������ [row][col] ((!)��������� � �������)
	QAxObject *cell = StatSheet->querySubObject("Cells(Int,Int)", row, col);
	// ������� �������� ���������� data (����� ���, ���������� � QVariant) � ���������� ������
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
	// �������� ��������� �� Excel
    QAxObject *mExcel = new QAxObject("Excel.Application",this);
	//������ ��� �������
	//mExcel->dynamicCall( "SetVisible(bool)", TRUE ); 
    // �������� ��������� �� �����
    QAxObject *workbooks = mExcel->querySubObject("Workbooks");
    // ��������� �����
	workbooks->dynamicCall("Add");
    // �������� ��������� �� ������� �����
	QAxObject *workbook = mExcel->querySubObject("ActiveWorkBook");
	if(!workbook)
		return 0;
    // �������� ��������� �� �����
    QAxObject *mSheets = workbook->querySubObject( "Sheets" );
    // ���������, ����� ���� �������
    QAxObject *StatSheet = mSheets->querySubObject( "Item(const QVariant&)", QVariant("����1") );
	if(!StatSheet)
		return 0;
	int r = 1;
#else
	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		QMessageBox::critical(NULL, QString("������"), QString("������ �������� �����!"));
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
				if(i==8)// �����
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
	//������ �����, ��������� ���������� �� �����
	tmp_s.replace("/","\\");
	//������� excel ��� ������ ��������� ��������� �������� �� ����
	mExcel->setProperty("DisplayAlerts", 0);
	//��������� ��� ���� ��� ��������������� ������ � ������� � ����������
	workbook->dynamicCall("SaveAs (const QString&)", tmp_s);

	mExcel->setProperty("DisplayAlerts", 1);

	//�����-�� ����, ��������� ����� �� ����������������, ������ �� ����
	workbook->dynamicCall("Close(Boolean)", false);

    delete StatSheet;
    delete mSheets;
    delete workbook;
    delete workbooks;
	delete mExcel;
#endif //SAVETOXML

	QMessageBox::information(this, QString("���������"), QString("��������� �������� ��������� � ����!"));
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
  num integer, -- ����� ��������
*/
	QList<int> hideColumns;
	QStringList headers;
	QList<int> cRem;
	
	hideColumns << 0;
#ifndef _DEBUG
	hideColumns << 10;
#endif

	headers <<  tr("id") << tr("���� ��������") << tr("���")  << tr("�����������") << tr("��.����") << tr("����������") << tr("������") << tr("������") << tr("�����") << tr("����") << tr("�����");

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

