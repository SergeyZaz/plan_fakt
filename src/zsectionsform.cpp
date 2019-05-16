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

void ZSectionsForm::setSectionsType(int indx)
{
	ui.cboType->setCurrentIndex(indx);
}

int ZSectionsForm::init( QSqlDatabase &database, const QString &table, int id )
{
	ZEditAbstractForm::init(database, table, id);

	ui.txtName->setFocus();

	QString stringQuery = QString("SELECT name,type,parent FROM sections WHERE id = %1").arg(curEditId);

	// new record
	if (curEditId == ADD_UNIC_CODE)
	{
		ui.txtName->setText("");
		ui.cboType->setCurrentIndex(0);
		ui.cboGroup->setCurrentIndex(0);
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
			ui.cboGroup->setCurrentIndex(query.value(2).toInt());
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

	if (curEditId == ADD_UNIC_CODE)
		stringQuery = QString("INSERT INTO sections ( name,type,parent ) VALUES (?, ?, ?)");
	else
		stringQuery = QString("UPDATE sections SET name=?, type=?, parent=? WHERE id=%1").arg(curEditId);

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
	query.addBindValue(ui.cboGroup->currentIndex());

	if(!query.exec())
	{
		QMessageBox::critical(this, tr("Ошибка"), query.lastError().text());
		return;
	}
	
	accept();
}