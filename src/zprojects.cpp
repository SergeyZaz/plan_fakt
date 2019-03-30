#include "zprojects.h"


ZProjects::ZProjects()
{
}

void ZProjects::initDB(QSqlDatabase &m_DB, const QString &m_TblName)
{
	m_tbl->setDatabase(m_DB);
/*
  id serial NOT NULL,
  name text, -- Название
  comment text, -- Комментарий
*/
	QList<int> hideColumns;
	QStringList headers;
	QList<int> cRem;
	
	hideColumns << 0;
	headers <<  tr("id") << tr("Название") << tr("Комментарий");

	m_tbl->setTable(m_TblName, headers, cRem);
	m_tbl->init(hideColumns);
}

