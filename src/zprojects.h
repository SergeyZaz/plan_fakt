#pragma once

#include "zmdichild.h"


class ZProjects : public ZMdiChild
{

public:
	ZProjects();

	void initDB(QSqlDatabase &m_DB, const QString &m_TblName);
};


