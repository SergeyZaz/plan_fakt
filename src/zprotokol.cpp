#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include "zprotokol.h"

ZProtokol::ZProtokol(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);

	loadItemsToListBox(ui.listUrPersons, "ur_persons");
	loadItemsToListBox(ui.listProjects, "projects");

	ui.dateStart->setDate(QDate::currentDate().addYears(-1));
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
			pItem->setCheckState(Qt::Checked);
			listBox->addItem(pItem);
		}
	}
}

void ZProtokol::saveProtokol()
{
	QString fileName = QFileDialog::getSaveFileName(this);
	if (fileName.isEmpty()) 
		return;

	if(!fileName.endsWith(".csv"))
		fileName += ".csv";

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
	case 0://по месяцам
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
	QStringList l_Mounths, l_Quarts;
	l_Mounths << "" << tr("Январь ") << tr("Февраль ") << tr("Март ") << tr("Апрель ") << tr("Май ") << tr("Июнь ") << tr("Июль ") << tr("Август ") << tr("Сентябрь ") << tr("Октябрь ") << tr("Ноябрь ") << tr("Декабрь ");
	l_Quarts << "I." << "II."<< "III."<< "IV.";
	QDate tmpDate;
	QString text;

	switch(m_Inderval)
	{
	case 0://по месяцам
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
		case 0://по месяцам
			text = l_Mounths.at(tmpDate.month()) + QString::number(tmpDate.year());
			tmpDate = tmpDate.addMonths(1);
			break;
		case 1://по кварталам
			text = l_Quarts.at(tmpDate.month() / 4) + QString::number(tmpDate.year());
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
	
	ui.tree->setColumnWidth(0, 200);

	puts("----------------");
	foreach(QDate d, l_Intervals)
		puts(d.toString("yyyy-MM-dd").toLocal8Bit().data());
	puts("++++++++++++++++");

	ui.tree->clear();
	
	QTreeWidgetItem *pItem, *pItemRoot;
	QSqlQuery query;
	int rc, id, j;
	double summa, summaPlus, summaMinus;
	QListWidgetItem *pListItem;
	QString sUrPersons, sProjects;
	QFont fnt;

	sUrPersons = "";
	for(i=0; i<ui.listUrPersons->count(); i++)
	{
		pListItem = ui.listUrPersons->item(i);
		if(pListItem->checkState() == Qt::Checked)
			sUrPersons += QString::number(pListItem->data(Qt::UserRole).toInt()) + ",";
	}
	sUrPersons.chop(1);

	sProjects = "";
	for(i=0; i<ui.listProjects->count(); i++)
	{
		pListItem = ui.listProjects->item(i);
		if(pListItem->checkState() == Qt::Checked)
			sProjects += QString::number(pListItem->data(Qt::UserRole).toInt()) + ",";
	}
	sProjects.chop(1);

	//Qt::UserRole = 0-Доходы/1-Расходы/2-Прибыль

	for(i=0; i<2; i++)
	{
		pItemRoot = new QTreeWidgetItem(ui.tree);
		pItemRoot->setText(0, i == 0 ? tr("Доходы") : tr("Расходы"));
		fnt = pItemRoot->font(0);
		fnt.setBold(true);
		pItemRoot->setFont(0, fnt);
		pItemRoot->setData(0, Qt::UserRole, i);
		ui.tree->addTopLevelItem(pItemRoot);

		pItem = new QTreeWidgetItem(pItemRoot);
		pItem->setText(0, i == 0 ? tr("Нераспределенный доход") : tr("Нераспределенный расход"));
		pItem->setData(0, Qt::UserRole, i);

		// нераспределенный расход это тоже что и нераспределенный доход только с типом операции = 1
		for(j=1; j<l_Intervals.size(); j++)
		{
			summa = 0;
			text = QString("SELECT val FROM operations WHERE type=%1 AND ur_person IN (%2) AND project IN (%3) AND date >= '%4' AND date < '%5' AND section=0")
				.arg(i)
				.arg(sUrPersons)
				.arg(sProjects)
				.arg(l_Intervals.at(j-1).toString("yyyy-MM-dd"))
				.arg(l_Intervals.at(j).toString("yyyy-MM-dd")
				.arg(id));
			if (query.exec(text))
			{
				while (query.next()) 
				{
					summa += query.value(0).toDouble();
				}
			}
			pItem->setText(j, QString::number(summa, 'f', 2));
		}

		rc = query.exec(QString("SELECT id,name FROM sections WHERE type=%1 AND id>0 ORDER BY name ").arg(i));
		if (rc)
		{
			while (query.next()) 
			{
				pItem = new QTreeWidgetItem(pItemRoot);
				pItem->setText(0, query.value(1).toString().simplified());
				pItem->setData(0, Qt::UserRole, i);
				id = query.value(0).toInt();
				pItem->setData(0, Qt::UserRole+1, id);

				for(j=1; j<l_Intervals.size(); j++)
				{
					summa = 0;
					text = QString("SELECT val FROM operations WHERE type=%1 AND ur_person IN (%2) AND project IN (%3) AND date >= '%4' AND date < '%5' AND section=%6")
						.arg(i)
						.arg(sUrPersons)
						.arg(sProjects)
						.arg(l_Intervals.at(j-1).toString("yyyy-MM-dd"))
						.arg(l_Intervals.at(j).toString("yyyy-MM-dd"))
						.arg(id);
					if (query.exec(text))
					{
						while (query.next()) 
						{
							summa += query.value(0).toDouble();
						}
					}
					pItem->setText(j, QString::number(summa, 'f', 2));
				}
			}
		}
	}

	QTreeWidgetItem *pItemItog = new QTreeWidgetItem(ui.tree);
	ui.tree->addTopLevelItem(pItemItog);
	pItemItog->setText(0, tr("Прибыль"));
	pItemItog->setData(0, Qt::UserRole, 2);
	pItemItog->setFont(0, fnt);
	
	summa = 0;
	for(i=1; i<l_Intervals.size(); i++)
	{
		for(j=0; j<2; j++)
		{
			pItemRoot = ui.tree->topLevelItem(j);
			if(!pItemRoot)
				continue;
			int n = pItemRoot->childCount();

			summa = 0;
			for(int ii=0; ii<n; ii++)
			{
				pItem = pItemRoot->child(ii);
				if(!pItem)
					continue;
				summa += pItem->text(i).toDouble();
			}

			pItemRoot->setText(i, QString::number(summa, 'f', 2));
			pItemRoot->setFont(i, fnt);

			if(j==0)
				summaPlus = summa;
			else
				summaMinus = summa;
		}
		pItemItog->setText(i, QString::number(summaPlus - summaMinus, 'f', 2));
		pItemItog->setFont(i, fnt);
		ui.tree->resizeColumnToContents(i);
	}
	
	ui.tree->expandAll();
}
	
int ZProtokol::saveToFile(const QString &fileName)
{
	int cols = ui.tree->headerItem()->columnCount();

	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		QMessageBox::critical(NULL, QString("Ошибка"), QString("Ошибка создания файла!"));
		return 0;
	}

     QTextStream out(&file);	 
	 int i, j, k, cilds, n = ui.tree->topLevelItemCount();
	 QTreeWidgetItem *pItemRoot, *pItem;

	 for(j=0; j<cols; j++)
	 {
		 out << ui.tree->headerItem()->text(j) << ";";
		 if(j == 0)
			 out << ";";
	 }
	 out << "\n";

	 for(i=0;i<n;i++)
	 {
		 pItemRoot = ui.tree->topLevelItem(i);
		 for(j=0; j<cols; j++)
		 {
			 out << pItemRoot->text(j) << ";";
			 if(j == 0)
				 out << ";";
		 }
		 out << "\n";

		 cilds = pItemRoot->childCount();
		 for(k=0; k<cilds; k++)
		 {
			 pItem = pItemRoot->child(k);
			 for(j=0; j<cols; j++)
			 {
				 if(j == 0)
					 out << ";";
				 out << pItem->text(j) << ";";
			 }
			 out << "\n";
		 }
	 }

	return 1;
}
