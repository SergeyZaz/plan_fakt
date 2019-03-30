#include "zurpersons.h"
#include "zurpersonsform.h"

ZUrPersons::ZUrPersons()
{
}


void ZUrPersons::initDB(QSqlDatabase &m_DB, const QString &m_TblName)
{
	m_tbl->setDatabase(m_DB);
/*
  id serial NOT NULL,
  name text, -- ������� ��������
  full_name text, -- ������ ��������
  comment text, -- �����������
  inn text, -- ���
  kpp text, -- ���
*/
	QList<int> hideColumns;
	QStringList headers;
	QList<int> cRem;
	
	hideColumns << 0;
	headers <<  tr("id") << tr("�������� (����.)") << tr("�������� (������)") << tr("�����������") << tr("���") << tr("���");

	m_tbl->setTable(m_TblName, headers, cRem);
	m_tbl->init(hideColumns);
	m_tbl->setCustomEditor(new ZUrPersonsForm);
	m_tbl->moveSection(3, 5);
}

