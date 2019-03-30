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
  parent integer, -- ��������� �:
*/
	QList<int> hideColumns;
	QStringList headers;
	QList<int> cRem;
	
	hideColumns << 0 << 3;
	headers <<  tr("id") << tr("��������") << tr("���");

	m_tbl->setTable(m_TblName, headers, cRem);
	m_tbl->setCustomEditor(new ZSectionsForm);
	
	QMap<int, QString> *pMap = new QMap<int, QString>;
	pMap->insert(0, tr("������"));
	pMap->insert(1, tr("�������"));
	m_tbl->setRelation(2, pMap);
	
	m_tbl->init(hideColumns);
}

