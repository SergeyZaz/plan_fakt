#pragma once

#include <QAction>
#include "zmdichild.h"


class ZOperations : public ZMdiChild
{
	QAction *actSetProject;
	QAction *actSetSection;
	void loadItemsToList(QStringList &list, const QString &tableName);
	int updateSelectedItems(const QString &elemName, const QString &tableName, const QString &attName);
	void execCustomAction(const QString &txt);

public:
    ZOperations();
 
	void initDB(QSqlDatabase &m_DB, const QString &m_TblName);
};


