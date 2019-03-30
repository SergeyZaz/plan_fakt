#include "zoperations.h"
#include "zoperationsform.h"

ZOperations::ZOperations()
{
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

