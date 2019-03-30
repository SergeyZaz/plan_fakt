#pragma once

#include <QShowEvent>
#include "ui_zaccountsform.h"
#include "zeditbaseform.h"

class ZAccountsForm : public ZEditAbstractForm
{
    Q_OBJECT
	Ui::ZAccountsForm ui;

public:
    ZAccountsForm();
	~ZAccountsForm();

	int init( QSqlDatabase &database, const QString &table, int id );
	void showEvent(QShowEvent*);

protected slots:
	void applyChanges();
	void addUrPerson();

};

