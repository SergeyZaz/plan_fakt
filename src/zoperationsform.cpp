#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include "zoperationsform.h"
#include "zurpersonsform.h"
#include "zsectionsform.h"
#include "zeditbaseform.h"
#include "zpartnersform.h"
#include "zaccountsform.h"

ZOperationsForm::ZOperationsForm()
{
	ui.setupUi(this);
	connect(ui.cmdSave, SIGNAL(clicked()), this, SLOT(applyChanges()));
	connect(ui.cmdAddUrPerson, SIGNAL(clicked()), this, SLOT(addUrPerson()));
	connect(ui.cmdAddPartner, SIGNAL(clicked()), this, SLOT(addAddPartner()));
	connect(ui.cmdAddSection, SIGNAL(clicked()), this, SLOT(addAddSection()));
	connect(ui.cmdAddProject, SIGNAL(clicked()), this, SLOT(addAddProject()));
	connect(ui.cmdAddAccount, SIGNAL(clicked()), this, SLOT(addAddAccount()));
	
	connect(ui.cboType, SIGNAL(currentIndexChanged(int)), this, SLOT(cboTypeChanged(int)));
	connect(ui.cboUrPerson, SIGNAL(currentIndexChanged(int)), this, SLOT(cboUrPersonChanged(int)));
	
	showEvent(NULL);
}

ZOperationsForm::~ZOperationsForm(){}

void ZOperationsForm::showEvent(QShowEvent*)
{
	ui.cboUrPerson->blockSignals(true);
	loadItemsToCombobox(ui.cboUrPerson, "ur_persons");
	ui.cboUrPerson->blockSignals(false);

	loadItemsToCombobox(ui.cboPartner, "partners");
	loadItemsToCombobox(ui.cboProject, "projects");
	cboTypeChanged(ui.cboType->currentIndex());
	cboUrPersonChanged(ui.cboUrPerson->currentIndex());
}

void ZOperationsForm::cboTypeChanged(int indx)
{
	if(indx != 2)
		loadItemsToCombobox(ui.cboSection, "sections", QString("type=%1").arg(indx));
	else
		loadItemsToCombobox(ui.cboSection, "sections");
}

void ZOperationsForm::cboUrPersonChanged(int indx)
{
	int id = ui.cboUrPerson->itemData(indx, Qt::UserRole).toInt();
	loadItemsToCombobox(ui.cboAccount, "accounts", QString("ur_person=%1").arg(id));
}

int ZOperationsForm::init( QSqlDatabase &database, const QString &table, int id )
{
	ZEditAbstractForm::init(database, table, id);

	ui.dateEdit->setFocus();
	ui.cboType->blockSignals(true);
	ui.cboUrPerson->blockSignals(true);

	QString stringQuery = QString("SELECT date,type,comment,ur_person,partner,section,project,account,val FROM operations WHERE id = %1").arg(curEditId);

	// new record
	if (curEditId == ADD_UNIC_CODE)
	{
		ui.dateEdit->setDate(QDate::currentDate());
		
		ui.cboType->setCurrentIndex(0);
		cboTypeChanged(0);

		ui.txtComment->setPlainText("");
		
		ui.cboUrPerson->setCurrentIndex(0);
		cboUrPersonChanged(0);

		ui.cboPartner->setCurrentIndex(0);
		ui.cboSection->setCurrentIndex(0);
		ui.cboProject->setCurrentIndex(0);
		ui.cboAccount->setCurrentIndex(0);
		ui.dValue->setValue(0);
		
		QApplication::processEvents();
		ui.cboType->blockSignals(false);
		ui.cboUrPerson->blockSignals(false);
		return true;
	}

	// execute request
	QSqlQuery query(m_DB);
	bool result = query.exec(stringQuery);
	if (result)
	{
		if (query.next()) 
		{
			ui.dateEdit->setDate(query.value(0).toDate());
			
			ui.cboType->setCurrentIndex(query.value(1).toInt());
			cboTypeChanged(ui.cboType->currentIndex());

			ui.txtComment->setPlainText(query.value(2).toString());
			
			ui.cboUrPerson->setCurrentIndex(ui.cboUrPerson->findData(query.value(3).toInt()));
			cboUrPersonChanged(ui.cboUrPerson->currentIndex());

			ui.cboPartner->setCurrentIndex(ui.cboPartner->findData(query.value(4).toInt()));
			ui.cboSection->setCurrentIndex(ui.cboSection->findData(query.value(5).toInt()));
			ui.cboProject->setCurrentIndex(ui.cboProject->findData(query.value(6).toInt()));
			ui.cboAccount->setCurrentIndex(ui.cboAccount->findData(query.value(7).toInt()));
			ui.dValue->setValue(query.value(8).toDouble());
		}
	}	
	else 
	{
		QMessageBox::critical(this, tr("Ошибка"), query.lastError().text());
	}

	QApplication::processEvents();
	ui.cboType->blockSignals(false);
	ui.cboUrPerson->blockSignals(false);
	return result;
}

void ZOperationsForm::applyChanges()
{
	QString text, stringQuery;
	int type = ui.cboType->currentIndex();

	if (curEditId == ADD_UNIC_CODE)
		stringQuery = QString("INSERT INTO operations ( date,type,comment,ur_person,partner,section,project,account,val ) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)");
	else
	{
//		if(type == 2) //перемещение
//			stringQuery = QString("UPDATE operations SET date=?, type=?, comment=?, ur_person=?, partner=?, project=?, val=? WHERE id=%1").arg(curEditId);
//		else
			stringQuery = QString("UPDATE operations SET date=?, type=?, comment=?, ur_person=?, partner=?, section=?, project=?, account=?, val=?  WHERE id=%1").arg(curEditId);
	}

	QSqlQuery query(m_DB);
	query.prepare(stringQuery);

	query.addBindValue(ui.dateEdit->date());
	query.addBindValue(type);

	text = ui.txtComment->toPlainText();
	if(text.isEmpty())
	{
		text = " ";
	}
	query.addBindValue(text);

	query.addBindValue(ui.cboUrPerson->itemData(ui.cboUrPerson->currentIndex(), Qt::UserRole));
	query.addBindValue(ui.cboPartner->itemData(ui.cboPartner->currentIndex(), Qt::UserRole));
	if(ui.cboSection->currentIndex() == -1)
	{
		QMessageBox::critical(this, tr("Ошибка"), tr("Не заполнено обязательное поле 'Статья'"));
		return;
	}
	query.addBindValue(ui.cboSection->itemData(ui.cboSection->currentIndex(), Qt::UserRole));
	query.addBindValue(ui.cboProject->itemData(ui.cboProject->currentIndex(), Qt::UserRole));
	if(ui.cboAccount->currentIndex() == -1)
	{
		QMessageBox::critical(this, tr("Ошибка"), tr("Не заполнено обязательное поле 'Счет'"));
		return;
	}
	int m_AccountId = ui.cboAccount->itemData(ui.cboAccount->currentIndex(), Qt::UserRole).toInt();
	query.addBindValue(m_AccountId);
	query.addBindValue(ui.dValue->value());

	if(!query.exec())
	{
		QMessageBox::critical(this, tr("Ошибка"), query.lastError().text());
		return;
	}
	
	emit needUpdateVal(m_AccountId);

	accept();
}

void ZOperationsForm::addUrPerson()
{
	ZEditAbstractForm *pD = new ZUrPersonsForm;
	pD->init(m_DB, "ur_persons", ADD_UNIC_CODE);
	pD->exec();
	loadItemsToCombobox(ui.cboUrPerson, "ur_persons");
}
	
void ZOperationsForm::addAddPartner()
{
	ZEditAbstractForm *pD = new ZPartnersForm;
	pD->init(m_DB, "partners", ADD_UNIC_CODE);
	pD->exec();
	loadItemsToCombobox(ui.cboPartner, "partners");
}
	
void ZOperationsForm::addAddSection()
{
	ZEditAbstractForm *pD = new ZSectionsForm;
	pD->init(m_DB, "sections", ADD_UNIC_CODE);
	pD->setSectionsType(ui.cboType->currentIndex());
	pD->exec();

	cboTypeChanged(ui.cboType->currentIndex());
}
	
void ZOperationsForm::addAddProject()
{
	ZEditAbstractForm *pD = new ZEditBaseForm;
	pD->init(m_DB, "projects", ADD_UNIC_CODE);
	pD->exec();
	loadItemsToCombobox(ui.cboProject, "projects");
}

void ZOperationsForm::addAddAccount()
{
	ZEditAbstractForm *pD = new ZAccountsForm;
	pD->init(m_DB, "accounts", ADD_UNIC_CODE);
	pD->setSectionsType(ui.cboType->currentIndex());
	pD->exec();

	cboTypeChanged(ui.cboType->currentIndex());
}
