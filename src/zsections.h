#pragma once

#include "zmdichild.h"


class ZSections : public ZMdiChild
{

public:
    ZSections();
 
	void initDB(QSqlDatabase &m_DB, const QString &m_TblName);
};


