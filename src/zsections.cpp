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
  name text, -- Название
  type integer, -- Тип: 0-Доходы/1-Расходы/2-Активы/3-Обязательства/4-Капитал
  parent integer, --группа: 0-операционный поток, 1-инвестиционный поток, 2-финансовый поток
*/
	QList<int> hideColumns;
	QStringList headers;
	QList<int> cRem;
	
	hideColumns << 0;
	headers <<  tr("id") << tr("Название") << tr("Тип") << tr("Группа");

	m_tbl->setTable(m_TblName, headers, cRem);
	m_tbl->setCustomEditor(new ZSectionsForm);
	
	QMap<int, QString> *pMap1 = new QMap<int, QString>;
	pMap1->insert(0, tr("Доходы"));
	pMap1->insert(1, tr("Расходы"));
	m_tbl->setRelation(2, pMap1);

	QMap<int, QString> *pMap2 = new QMap<int, QString>;
	pMap2->insert(0, tr("операционный поток"));
	pMap2->insert(1, tr("инвестиционный поток"));
	pMap2->insert(2, tr("финансовый поток"));
	m_tbl->setRelation(3, pMap2);
	
	m_tbl->init(hideColumns);
}

