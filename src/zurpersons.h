#pragma once

#include "zmdichild.h"


class ZUrPersons : public ZMdiChild
{

public:
    ZUrPersons();
 
	void initDB(QSqlDatabase &m_DB, const QString &m_TblName);
};


