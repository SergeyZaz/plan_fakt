#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include "zurpersonsform.h"

ZUrPersonsForm::ZUrPersonsForm()
{
	ui.setupUi(this);
	connect(ui.cmdSave, SIGNAL(clicked()), this, SLOT(applyChanges()));
}

ZUrPersonsForm::~ZUrPersonsForm(){}

int ZUrPersonsForm::init( QSqlDatabase &database, const QString &table, int id )
{
	ZEditAbstractForm::init(database, table, id);

	ui.txtName->setFocus();

	QString stringQuery = QString("SELECT name,full_name,comment,inn,kpp FROM ur_persons WHERE id = %1").arg(curEditId);

	// new record
	if (curEditId == ADD_UNIC_CODE)
	{
		ui.txtName->setText("");
		ui.txtNameFull->setText("");
		ui.txtComment->setText("");
		ui.txtINN->setText("");
		ui.txtKPP->setText("");
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
			ui.txtNameFull->setText(query.value(1).toString());
			ui.txtComment->setText(query.value(2).toString());
			ui.txtINN->setText(query.value(3).toString());
			ui.txtKPP->setText(query.value(4).toString());
		}
	}	
	else 
	{
		QMessageBox::critical(this, tr("Ошибка"), query.lastError().text());
	}

	return result;
}

void ZUrPersonsForm::applyChanges()
{
	QString text, stringQuery;

	if (curEditId == ADD_UNIC_CODE)
		stringQuery = QString("INSERT INTO ur_persons ( name,full_name,comment,inn,kpp ) VALUES (?, ?, ?, ?, ?)");
	else
		stringQuery = QString("UPDATE ur_persons SET name=?, full_name=?, comment=?, inn=? , kpp=? WHERE id=%1").arg(curEditId);

	QSqlQuery query(m_DB);
	query.prepare(stringQuery);

	text = ui.txtName->text();
	if(text.isEmpty())
	{
		QMessageBox::critical(this, tr("Ошибка"), tr("Не заполнено обязательное поле 'Краткое наименование'"));
		return;
	}
	query.addBindValue(text);

	text = ui.txtNameFull->text();
	if(text.isEmpty())
	{
		text = " ";
	}
	query.addBindValue(text);

	text = ui.txtComment->toPlainText();
	if(text.isEmpty())
	{
		text = " ";
	}
	query.addBindValue(text);

	text = ui.txtINN->text();
	if(text.isEmpty())
	{
		QMessageBox::critical(this, tr("Ошибка"), tr("Не заполнено обязательное поле 'ИНН'"));
		return;
	}
	query.addBindValue(text);

	text = ui.txtKPP->text();
	if(text.isEmpty())
	{
		text = " ";
	}
	query.addBindValue(text);

	if(!query.exec())
	{
		QMessageBox::critical(this, tr("Ошибка"), query.lastError().text());
		return;
	}
	
	accept();
}