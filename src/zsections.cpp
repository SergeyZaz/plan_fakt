#include "zsections.h"
#include "zsectionsform.h"

ZSections::ZSections()
{
}

void ZSections::initDB(QSqlDatabase &m_DB, const QString &m_TblName)
{
	m_tbl->setDatabase(m_DB);
/*
  id serial NOT NULL,
  name text, -- ��������
  type integer, -- ���: 0-������/1-�������/2-������/3-�������������/4-�������
  parent integer, --������: 0-������������ �����, 1-�������������� �����, 2-���������� �����
*/
	QList<int> hideColumns;
	QStringList headers;
	QList<int> cRem;
	
	hideColumns << 0;
	headers <<  tr("id") << tr("��������") << tr("���") << tr("������");

	m_tbl->setTable(m_TblName, headers, cRem);
	m_tbl->setCustomEditor(new ZSectionsForm);
	
	QMap<int, QString> *pMap1 = new QMap<int, QString>;
	pMap1->insert(0, tr("������"));
	pMap1->insert(1, tr("�������"));
	m_tbl->setRelation(2, pMap1);

	QMap<int, QString> *pMap2 = new QMap<int, QString>;
	pMap2->insert(0, tr("������������ �����"));
	pMap2->insert(1, tr("�������������� �����"));
	pMap2->insert(2, tr("���������� �����"));
	m_tbl->setRelation(3, pMap2);
	
	m_tbl->init(hideColumns);
}

