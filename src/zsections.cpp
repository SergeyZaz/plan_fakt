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
  parent integer, -- Относится к:
*/
	QList<int> hideColumns;
	QStringList headers;
	QList<int> cRem;
	
	hideColumns << 0 << 3;
	headers <<  tr("id") << tr("Название") << tr("Тип");

	m_tbl->setTable(m_TblName, headers, cRem);
	m_tbl->setCustomEditor(new ZSectionsForm);
	
	QMap<int, QString> *pMap = new QMap<int, QString>;
	pMap->insert(0, tr("Доходы"));
	pMap->insert(1, tr("Расходы"));
	m_tbl->setRelation(2, pMap);
	
	m_tbl->init(hideColumns);
}

