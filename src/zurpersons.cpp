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
  name text, -- Краткое название
  full_name text, -- Полное название
  comment text, -- Комментарий
  inn text, -- ИНН
  kpp text, -- КПП
*/
	QList<int> hideColumns;
	QStringList headers;
	QList<int> cRem;
	
	hideColumns << 0;
	headers <<  tr("id") << tr("Название (сокр.)") << tr("Название (полное)") << tr("Комментарий") << tr("ИНН") << tr("КПП");

	m_tbl->setTable(m_TblName, headers, cRem);
	m_tbl->init(hideColumns);
	m_tbl->setCustomEditor(new ZUrPersonsForm);
	m_tbl->moveSection(3, 5);
}

