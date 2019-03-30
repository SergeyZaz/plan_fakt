#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QFileDialog>
#include "zprotokol.h"

ZProtokol::ZProtokol(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);

	loadItemsToListBox(ui.listUrPersons, "ur_persons");
	loadItemsToListBox(ui.listProjects, "projects");

	ui.dateStart->setDate(QDate::currentDate());
	ui.dateEnd->setDate(QDate::currentDate());

	setModal(true);

	connect(ui.cmdBuild, SIGNAL(clicked()), this, SLOT(buildProtokol()));
	connect(ui.cmdSave, SIGNAL(clicked()), this, SLOT(saveProtokol()));
}

ZProtokol::~ZProtokol()
{

}

void ZProtokol::loadItemsToListBox( QListWidget *listBox, const QString &tableName)
{
	QSqlQuery query;

	// clear box
	listBox->clear();
	auto result = query.exec(QString("SELECT id, name FROM %1 ORDER BY name").arg(tableName));
	if (result)
	{
		while (query.next()) 
		{
			QListWidgetItem *pItem = new QListWidgetItem(query.value(1).toString().simplified());
			pItem->setData(Qt::UserRole, query.value(0).toInt());
			pItem->setCheckState(Qt::Unchecked);
			listBox->addItem(pItem);
		}
	}
}

void ZProtokol::saveProtokol()
{
	QString fileName = QFileDialog::getSaveFileName(this);
	if (fileName.isEmpty()) 
		return;
	saveToFile(fileName);
}

void ZProtokol::buildProtokol()
{
	int m_Inderval = ui.cboInterval->currentIndex();
	QDate m_dStart = ui.dateStart->date();
	QDate m_dStop = ui.dateEnd->date();

	int i,numInterval;

	switch(m_Inderval)
	{
	case 0://по мес€цам
		numInterval = (m_dStop.year() - m_dStart.year()) * 12 + m_dStop.month() - m_dStart.month() + 1;
		break;
	case 1://по кварталам
		numInterval = (m_dStop.year() - m_dStart.year()) * 4 + (m_dStop.month() / 4)  - (m_dStart.month() / 4) + 1;
		break;
	case 2://по годам
		numInterval = m_dStop.year() - m_dStart.year() + 1;
		break;
	default:
		return;
	}

	ui.tree->setColumnCount(numInterval + 1);
	QTreeWidgetItem *pHeader = ui.tree->headerItem();

	QList<QDate> l_Intervals;
	QStringList strMounths, strQuarts;
	strMounths << "" << tr("январь ") << tr("‘евраль ") << tr("ћарт ") << tr("јпрель ") << tr("ћай ") << tr("»юнь ") << tr("»юль ") << tr("јвгуст ") << tr("—ент€брь ") << tr("ќкт€брь ") << tr("Ќо€брь ") << tr("ƒекабрь ");
	strQuarts << "I." << "II."<< "III."<< "IV.";
	QDate tmpDate;
	QString text;

	switch(m_Inderval)
	{
	case 0://по мес€цам
		tmpDate = QDate(m_dStart.year(), m_dStart.month(), 1);
		l_Intervals << tmpDate;
		break;
	case 1://по кварталам
		tmpDate = QDate(m_dStart.year(), (m_dStart.month() / 4) + 1, 1);
		l_Intervals << tmpDate;
		break;
	case 2://по годам
		tmpDate = QDate(m_dStart.year(), 1, 1);
		l_Intervals << tmpDate;
		break;
	}

	for(i=0; i<numInterval; i++)
	{
		switch(m_Inderval)
		{
		case 0://по мес€цам
			text = strMounths.at(tmpDate.month()) + QString::number(tmpDate.year());
			tmpDate = tmpDate.addMonths(1);
			break;
		case 1://по кварталам
			text = strQuarts.at(tmpDate.month() / 4) + QString::number(tmpDate.year());
			tmpDate = tmpDate.addMonths(3);
			break;
		case 2://по годам
			text = QString::number(tmpDate.year());
			tmpDate = tmpDate.addMonths(12);
			break;
		}
		l_Intervals << tmpDate;
		pHeader->setText(i+1, text);
	}

	puts("----------------");
	foreach(QDate d, l_Intervals)
		puts(d.toString("yyyy-MM-dd").toLocal8Bit().data());
	puts("++++++++++++++++");

	ui.tree->clear();
	ui.tree->resizeColumnToContents(0);
	
	//...
}
	
int ZProtokol::saveToFile(const QString &fileName)
{
	//...
	return 1;
}
