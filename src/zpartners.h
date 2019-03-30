#pragma once

#include "zmdichild.h"


class ZPartners : public ZMdiChild
{

public:
    ZPartners();
 
	void initDB(QSqlDatabase &m_DB, const QString &m_TblName);
};


