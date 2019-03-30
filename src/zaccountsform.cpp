#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include "zaccountsform.h"
#include "zurpersonsform.h"

ZAccountsForm::ZAccountsForm()
{
	ui.setupUi(this);
	connect(ui.cmdSave, SIGNAL(clicked()), this, SLOT(applyChanges()));
	connect(ui.cmdAddUrPerson, SIGNAL(clicked()), this, SLOT(addUrPerson()));
}

ZAccountsForm::~ZAccountsForm(){}

void ZAccountsForm::showEvent(QShowEvent*)
{
	loadItemsToCombobox(ui.cboUrPerson, "ur_persons");
}

int ZAccountsForm::init( QSqlDatabase &database, const QString &table, int id )
{
	ZEditAbstractForm::init(database, table, id);

	ui.txtName->setFocus();

	QString stringQuery = QString("SELECT name,comment,type,ur_person,bik,bank,account,val FROM accounts WHERE id = %1").arg(curEditId);

	// new record
	if (curEditId == -1)
	{
		ui.txtName->setText("");
		ui.txtComment->setText("");
		ui.cboType->setCurrentIndex(0);
		ui.cboUrPerson->setCurrentIndex(0);
		ui.txtBIK->setText("");
		ui.txtBank->setText("");
		ui.txtAccount->setText("");
		ui.dValue->setValue(0);
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
			ui.txtComment->setText(query.value(1).toString());
			ui.cboType->setCurrentIndex(query.value(2).toInt());
			ui.cboUrPerson->setCurrentIndex(ui.cboUrPerson->findData(query.value(3).toInt()));
			ui.txtBIK->setText(query.value(4).toString());
			ui.txtBank->setText(query.value(5).toString());
			ui.txtAccount->setText(query.value(6).toString());
			ui.dValue->setValue(query.value(7).toDouble());
		}
	}	
	else 
	{
		QMessageBox::critical(this, tr("Ошибка"), query.lastError().text());
	}

	return result;
}

void ZAccountsForm::applyChanges()
{
	QString text, stringQuery;

	if (curEditId == -1)
		stringQuery = QString("INSERT INTO accounts ( name,comment,type,ur_person,bik,bank,account,val ) VALUES (?, ?, ?, ?, ?, ?, ?, ?)");
	else
		stringQuery = QString("UPDATE accounts SET name=?, comment=?, type=?, ur_person=?, bik=?, bank=?, account=?, val=?  WHERE id=%1").arg(curEditId);

	QSqlQuery query(m_DB);
	query.prepare(stringQuery);

	text = ui.txtName->text();
	if(text.isEmpty())
	{
		QMessageBox::critical(this, tr("Ошибка"), tr("Не заполнено обязательное поле 'Краткое наименование'"));
		return;
	}
	query.addBindValue(text);

	text = ui.txtComment->toPlainText();
	if(text.isEmpty())
	{
		text = " ";
	}
	query.addBindValue(text);

	query.addBindValue(ui.cboType->currentIndex());
	query.addBindValue(ui.cboUrPerson->itemData(ui.cboUrPerson->currentIndex(), Qt::UserRole));

	text = ui.txtBIK->text();
	if(text.isEmpty())
	{
		QMessageBox::critical(this, tr("Ошибка"), tr("Не заполнено обязательное поле 'БИК'"));
		return;
	}
	query.addBindValue(text);

	text = ui.txtBank->text();
	if(text.isEmpty())
	{
		QMessageBox::critical(this, tr("Ошибка"), tr("Не заполнено обязательное поле 'Банк'"));
		return;
	}
	query.addBindValue(text);

	text = ui.txtAccount->text();
	if(text.isEmpty())
	{
		QMessageBox::critical(this, tr("Ошибка"), tr("Не заполнено обязательное поле 'Номер счета'"));
		return;
	}
	query.addBindValue(text);

	query.addBindValue(ui.dValue->value());

	if(!query.exec())
	{
		QMessageBox::critical(this, tr("Ошибка"), query.lastError().text());
		return;
	}
	
	accept();
}

void ZAccountsForm::addUrPerson()
{
	ZEditAbstractForm *pD = new ZUrPersonsForm;
	pD->init(m_DB, "ur_persons", -1);
	pD->exec();
	loadItemsToCombobox(ui.cboUrPerson, "ur_persons");
}
