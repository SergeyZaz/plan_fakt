#ifndef ZMDICHILD_H
#define ZMDICHILD_H

#include <QDialog>
#include <QSqlDatabase>
#include "zview.h"

class ZMdiChild : public QDialog
{
    Q_OBJECT

public:
    ZMdiChild();
 
	virtual void initDB(QSqlDatabase &, const QString &){};
	// set icon and title
	virtual void setWindowTitleAndIcon(const QString &title, const QIcon &icon) 
	{
		setWindowTitle(title);
		setWindowIcon(icon);
	}
	void reload() {if(m_tbl) m_tbl->reload();}
protected:
    void closeEvent(QCloseEvent *event);
	ZView *m_tbl;

signals:
	void needUpdate();
};

#endif
