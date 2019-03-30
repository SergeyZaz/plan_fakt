#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include "zsectionsform.h"

ZSectionsForm::ZSectionsForm()
{
	ui.setupUi(this);
	connect(ui.cmdSave, SIGNAL(clicked()), this, SLOT(applyChanges()));
}

ZSectionsForm::~ZSectionsForm(){}

int ZSectionsForm::init( QSqlDatabase &database, const QString &table, int id )
{
	ZEditAbstractForm::init(database, table, id);

	ui.txtName->setFocus();

	QString stringQuery = QString("SELECT name,type FROM sections WHERE id = %1").arg(curEditId);

	// new record
	if (curEditId == -1)
	{
		ui.txtName->setText("");
		ui.cboType->setCurrentIndex(0);
		return true;
	}

	// execute request
	QSqlQuery query(m_DB);
	bool result = query.exec(stringQuery);
	if (result)
	{
		if (query.next()) 
		{
			ui.txtName->setText(query.value(0).toString());
			ui.cboType->setCurrentIndex(query.value(1).toInt());
		}
	}	
	else 
	{
		QMessageBox::critical(this, tr("Ошибка"), query.lastError().text());
	}

	return result;
}

void ZSectionsForm::applyChanges()
{
	QString text, stringQuery;

	if (curEditId == -1)
		stringQuery = QString("INSERT INTO sections ( name,type ) VALUES (?, ?)");
	else
		stringQuery = QString("UPDATE sections SET name=?, type=? WHERE id=%1").arg(curEditId);

	QSqlQuery query(m_DB);
	query.prepare(stringQuery);

	text = ui.txtName->text();
	if(text.isEmpty())
	{
		QMessageBox::critical(this, tr("Ошибка"), tr("Не заполнено обязательное поле 'Наименование'"));
		return;
	}
	query.addBindValue(text);

	query.addBindValue(ui.cboType->currentIndex());

	if(!query.exec())
	{
		QMessageBox::critical(this, tr("Ошибка"), query.lastError().text());
		return;
	}
	
	accept();
}