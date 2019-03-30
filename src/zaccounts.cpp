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
  name text, -- Краткое название
  comment text, -- Комментарий
  type integer, -- Тип: 0-наличный/1-безналичный
  ur_person integer, -- Юр.лицо
  bik text, -- БИК
  bank text, -- Банк
  account text, -- Номер счета
  val double precision, -- Текущий остаток
*/
	QList<int> hideColumns;
	QStringList headers;
	QList<int> cRem;
	
	hideColumns << 0;
	headers <<  tr("id") << tr("Название") << tr("Комментарий") << tr("Тип") << tr("Юр.лицо") << tr("БИК") << tr("Банк") << tr("Номер счета") << tr("Текущий остаток");

	m_tbl->setTable(m_TblName, headers, cRem);
	m_tbl->setCustomEditor(new ZAccountsForm);
	
	QMap<int, QString> *pMap = new QMap<int, QString>;
	pMap->insert(0, tr("наличный"));
	pMap->insert(1, tr("безналичный"));
	m_tbl->setRelation(3, pMap);
	
	m_tbl->setRelation(4, "ur_persons", "id", "name");
	
	m_tbl->init(hideColumns);
	m_tbl->moveSection(2, 8);
}

