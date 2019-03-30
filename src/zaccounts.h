#pragma once

#include "zmdichild.h"


class ZAccounts : public ZMdiChild
{

public:
    ZAccounts();
 
	void initDB(QSqlDatabase &m_DB, const QString &m_TblName);
};


