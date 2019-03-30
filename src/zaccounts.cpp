#include "zaccounts.h"
#include "zaccountsform.h"

ZAccounts::ZAccounts()
{
}


void ZAccounts::initDB(QSqlDatabase &m_DB, const QString &m_TblName)
{
	m_tbl->setDatabase(m_DB);
/*
  id serial NOT NULL,
  name text, -- ������� ��������
  comment text, -- �����������
  type integer, -- ���: 0-��������/1-�����������
  ur_person integer, -- ��.����
  bik text, -- ���
  bank text, -- ����
  account text, -- ����� �����
  val double precision, -- ������� �������
*/
	QList<int> hideColumns;
	QStringList headers;
	QList<int> cRem;
	
	hideColumns << 0;
	headers <<  tr("id") << tr("��������") << tr("�����������") << tr("���") << tr("��.����") << tr("���") << tr("����") << tr("����� �����") << tr("������� �������");

	m_tbl->setTable(m_TblName, headers, cRem);
	m_tbl->setCustomEditor(new ZAccountsForm);
	
	QMap<int, QString> *pMap = new QMap<int, QString>;
	pMap->insert(0, tr("��������"));
	pMap->insert(1, tr("�����������"));
	m_tbl->setRelation(3, pMap);
	
	m_tbl->setRelation(4, "ur_persons", "id", "name");
	
	m_tbl->init(hideColumns);
	m_tbl->moveSection(2, 8);
}

