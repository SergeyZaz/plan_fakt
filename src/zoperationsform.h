#pragma once

#include "ui_zoperationsform.h"
#include "zeditbaseform.h"

class ZOperationsForm : public ZEditAbstractForm
{
    Q_OBJECT
	Ui::ZOperationsForm ui;

public:
    ZOperationsForm();
	~ZOperationsForm();

	int init( QSqlDatabase &database, const QString &table, int id );
	void showEvent(QShowEvent*); 

protected slots:
	void applyChanges();
	void addUrPerson();
	void addAddPartner();
	void addAddSection();
	void addAddProject();
	void addAddAccount();
	void cboTypeChanged(int);
	void cboUrPersonChanged(int);

};

