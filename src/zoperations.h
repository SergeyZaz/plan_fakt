#pragma once

#include <QAction>
#include "zmdichild.h"


class ZOperations : public ZMdiChild
{
	QAction *actSetProject;
	QAction *actSetSection;
	void loadItemsToList(QStringList &l_Text, QList<int> &l_id, const QString &tableName);
	int updateSelectedItems(int setId, const QString &attName);
	void execCustomAction(const QString &txt);

public:
    ZOperations();
 
	void initDB(QSqlDatabase &m_DB, const QString &m_TblName);
};


