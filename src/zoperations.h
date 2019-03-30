#pragma once

#include "zmdichild.h"


class ZOperations : public ZMdiChild
{

public:
    ZOperations();
 
	void initDB(QSqlDatabase &m_DB, const QString &m_TblName);
};


