#include "zpartners.h"
#include "zpartnersform.h"

ZPartners::ZPartners()
{
}


void ZPartners::initDB(QSqlDatabase &m_DB, const QString &m_TblName)
{
	m_tbl->setDatabase(m_DB);
/*
  id serial NOT NULL,
  name text, -- ������� ��������
  full_name text, -- ������ ��������
  comment text, -- �����������
  inn text, -- ���
  kpp text, -- ���
  section_plus integer DEFAULT 0, -- �������� ������ �� ���������
  section_minus integer DEFAULT (-1), -- ��������� ������ �� ���������
*/
	QList<int> hideColumns;
	QStringList headers;
	QList<int> cRem;
	QSqlQuery query;
	
	hideColumns << 0;
	headers <<  tr("id") << tr("�������� (����.)") << tr("�������� (������)") << tr("�����������") 
		<< tr("���") << tr("���") << tr("�������� ������") << tr("��������� ������");
	
	m_tbl->setTable(m_TblName, headers, cRem);
	m_tbl->setCustomEditor(new ZPartnersForm);

	////////// �������� ������ �� ��������� //////////
	QMap<int, QString> *pMap0 = new QMap<int, QString>;
	if (query.exec(QString("SELECT id,name FROM sections WHERE type=0")))
	{
		while (query.next()) 
		{
			pMap0->insert(query.value(0).toInt(), query.value(1).toString());
		}
	}
	m_tbl->setRelation(6, pMap0);
	
	////////// ��������� ������ �� ��������� //////////
	QMap<int, QString> *pMap1 = new QMap<int, QString>;
	if (query.exec(QString("SELECT id,name FROM sections WHERE type=1")))
	{
		while (query.next()) 
		{
			pMap1->insert(query.value(0).toInt(), query.value(1).toString());
		}
	}
	m_tbl->setRelation(7, pMap1);

	m_tbl->init(hideColumns);
	m_tbl->moveSection(3, 5);
}

