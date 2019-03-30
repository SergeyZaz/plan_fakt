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
  date text, -- Дата операции
  type integer, -- Тип: 0-Поступление/1-Выплата/2-Перемещение
  comment text, -- Комментарий
  ur_person integer, -- Юр.лицо
  partner integer, -- контрагент
  section integer, -- статья
  project integer, -- проект
  val double precision, -- Сумма
*/
	QList<int> hideColumns;
	QStringList headers;
	QList<int> cRem;
	
	hideColumns << 0;
	headers <<  tr("id") << tr("Дата операции") << tr("Тип")  << tr("Комментарий") << tr("Юр.лицо") << tr("Контрагент") << tr("Статья") << tr("Проект") << tr("Сумма");

	m_tbl->setTable(m_TblName, headers, cRem);
	m_tbl->setCustomEditor(new ZOperationsForm);
	
	QMap<int, QString> *pMap = new QMap<int, QString>;
	pMap->insert(0, tr("Поступление"));
	pMap->insert(1, tr("Выплата"));
	pMap->insert(2, tr("Перемещение"));
	m_tbl->setRelation(2, pMap);
	
	m_tbl->setRelation(4, "ur_persons", "id", "name");
	m_tbl->setRelation(5, "partners", "id", "name");
	m_tbl->setRelation(6, "sections", "id", "name");
	m_tbl->setRelation(7, "projects", "id", "name");
	
	m_tbl->init(hideColumns);
	m_tbl->moveSection(3, 8);
}

