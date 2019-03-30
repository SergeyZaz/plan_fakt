#pragma once

#include "ui_zurpersonsform.h"
#include "zeditbaseform.h"

class ZUrPersonsForm : public ZEditAbstractForm
{
    Q_OBJECT
	Ui::ZUrPersonsForm ui;

public:
    ZUrPersonsForm();
	~ZUrPersonsForm();

	int init( QSqlDatabase &database, const QString &table, int id );

protected slots:
	void applyChanges();

};

