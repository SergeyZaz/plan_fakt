#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include "zoperationsform.h"
#include "zurpersonsform.h"
#include "zsectionsform.h"
#include "zeditbaseform.h"
#include "zpartnersform.h"

ZOperationsForm::ZOperationsForm()
{
	ui.setupUi(this);
	connect(ui.cmdSave, SIGNAL(clicked()), this, SLOT(applyChanges()));
	connect(ui.cmdAddUrPerson, SIGNAL(clicked()), this, SLOT(addUrPerson()));
	connect(ui.cmdAddPartner, SIGNAL(clicked()), this, SLOT(addAddPartner()));
	connect(ui.cmdAddSection, SIGNAL(clicked()), this, SLOT(addAddSection()));
	connect(ui.cmdAddProject, SIGNAL(clicked()), this, SLOT(addAddProject()));
	showEvent(NULL);
}

ZOperationsForm::~ZOperationsForm(){}

void ZOperationsForm::showEvent(QShowEvent*)
{

	loadItemsToCombobox(ui.cboUrPerson, "ur_persons");
	loadItemsToCombobox(ui.cboPartner, "partners");
	loadItemsToCombobox(ui.cboSection, "sections");
	loadItemsToCombobox(ui.cboProject, "projects");
}


int ZOperationsForm::init( QSqlDatabase &database, const QString &table, int id )
{
	ZEditAbstractForm::init(database, table, id);

	ui.dateEdit->setFocus();

	QString stringQuery = QString("SELECT date,type,comment,ur_person,partner,section,project,val FROM operations WHERE id = %1").arg(curEditId);

	// new record
	if (curEditId == ADD_UNIC_CODE)
	{
		ui.dateEdit->setDate(QDate::currentDate());
		ui.cboType->setCurrentIndex(0);
		ui.txtComment->setPlainText("");
		ui.cboUrPerson->setCurrentIndex(0);
		ui.cboPartner->setCurrentIndex(0);
		ui.cboSection->setCurrentIndex(0);
		ui.cboProject->setCurrentIndex(0);
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
			ui.dateEdit->setDate(query.value(0).toDate());
			ui.cboType->setCurrentIndex(query.value(1).toInt());
			ui.txtComment->setPlainText(query.value(2).toString());
			ui.cboUrPerson->setCurrentIndex(ui.cboUrPerson->findData(query.value(3).toInt()));
			ui.cboPartner->setCurrentIndex(ui.cboPartner->findData(query.value(4).toInt()));
			ui.cboSection->setCurrentIndex(ui.cboSection->findData(query.value(5).toInt()));
			ui.cboProject->setCurrentIndex(ui.cboProject->findData(query.value(6).toInt()));
			ui.dValue->setValue(query.value(7).toDouble());
		}
	}	
	else 
	{
		QMessageBox::critical(this, tr("Îøèáêà"), query.lastError().text());
	}

	return result;
}

void ZOperationsForm::applyChanges()
{
	QString text, stringQuery;

	if (curEditId == ADD_UNIC_CODE)
		stringQuery = QString("INSERT INTO operations ( date,type,comment,ur_person,partner,section,project,val ) VALUES (?, ?, ?, ?, ?, ?, ?, ?)");
	else
		stringQuery = QString("UPDATE operations SET date=?, type=?, comment=?, ur_person=?, partner=?, section=?, project=?, val=?  WHERE id=%1").arg(curEditId);

	QSqlQuery query(m_DB);
	query.prepare(stringQuery);

	query.addBindValue(ui.dateEdit->date());
	query.addBindValue(ui.cboType->currentIndex());

	text = ui.txtComment->toPlainText();
	if(text.isEmpty())
	{
		text = " ";
	}
	query.addBindValue(text);

	query.addBindValue(ui.cboUrPerson->itemData(ui.cboUrPerson->currentIndex(), Qt::UserRole));
	query.addBindValue(ui.cboPartner->itemData(ui.cboPartner->currentIndex(), Qt::UserRole));
	query.addBindValue(ui.cboSection->itemData(ui.cboSection->currentIndex(), Qt::UserRole));
	query.addBindValue(ui.cboProject->itemData(ui.cboProject->currentIndex(), Qt::UserRole));
	query.addBindValue(ui.dValue->value());

	if(!query.exec())
	{
		QMessageBox::critical(this, tr("Îøèáêà"), query.lastError().text());
		return;
	}
	
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
	pD->exec();
	loadItemsToCombobox(ui.cboSection, "sections");
}
	
void ZOperationsForm::addAddProject()
{
	ZEditAbstractForm *pD = new ZEditBaseForm;
	pD->init(m_DB, "projects", ADD_UNIC_CODE);
	pD->exec();
	loadItemsToCombobox(ui.cboProject, "projects");
}
