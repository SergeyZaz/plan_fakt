#pragma once

#include "ui_zsectionsform.h"
#include "zeditbaseform.h"

class ZSectionsForm : public ZEditAbstractForm
{
    Q_OBJECT
	Ui::ZSectionsForm ui;

public:
    ZSectionsForm();
	~ZSectionsForm();

	int init( QSqlDatabase &database, const QString &table, int id );
	void setSectionsType(int indx);

protected slots:
	void applyChanges();

};

