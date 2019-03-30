#pragma once

#include "ui_zpartnersform.h"
#include "zeditbaseform.h"

class ZPartnersForm : public ZEditAbstractForm
{
    Q_OBJECT
	Ui::ZPartnersForm ui;

public:
    ZPartnersForm();
	~ZPartnersForm();

	int init( QSqlDatabase &database, const QString &table, int id );

protected slots:
	void applyChanges();

};

