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
	
	//ui.dateStart->setDate(QDate(2019, 01, 01));
	//ui.dateEnd->setDate(QDate(2019, 01, 31));

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
	
	QTreeWidgetItem *pItem, *pItemRoot, *pItemGroup;
	QSqlQuery query1, query2;
	int rc, id, j;
	double summa;
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

	for(int ii=0; ii<3; ii++)
	{
		pItemGroup = new QTreeWidgetItem(ui.tree);
		switch(ii)
		{
		case 1:
			pItemGroup->setText(0, tr("инвестиционный поток"));
			break;
		case 2:
			pItemGroup->setText(0, tr("финансовый поток"));
			break;
		default:
			pItemGroup->setText(0, tr("операционный поток"));
			break;
		}
		fnt = pItemGroup->font(0);
		fnt.setBold(true);
		pItemGroup->setFont(0, fnt);
		//pItemGroup->setData(0, Qt::UserRole+1, ii);
		ui.tree->addTopLevelItem(pItemGroup);
		
		for(i=0; i<2; i++)
		{
			pItemRoot = new QTreeWidgetItem(pItemGroup);
			pItemRoot->setText(0, i == 0 ? tr("Поступления") : tr("Выплаты"));
			pItemRoot->setData(0, Qt::UserRole, i == 0 ? 1 : -1);
			ui.tree->addTopLevelItem(pItemRoot);

			rc = query1.exec(QString("SELECT id,name FROM sections WHERE type=%1 AND parent=%2 ORDER BY name ").arg(i).arg(ii));
			if (rc)
			{
				while (query1.next()) 
				{
					pItem = new QTreeWidgetItem(pItemRoot);
					pItem->setText(0, query1.value(1).toString().simplified());
					pItem->setData(0, Qt::UserRole, i == 0 ? 1 : -1);
					id = query1.value(0).toInt();
					//pItem->setData(0, Qt::UserRole+1, id);

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
						if (query2.exec(text))
						{
							while (query2.next()) 
							{
								summa += query2.value(0).toDouble();
							}
						}

#ifndef MONEY_FORMAT
						pItem->setText(j, QString::number(summa, 'f', 2));
#else
						pItem->setText(j, QString("%L1").arg(summa, 0, 'f', 2));
#endif
						pItem->setTextAlignment(j, Qt::AlignRight);
					}
				}
			}
		}
	}

	//////////////////////// перемещения ////////////////////////
	pItemGroup = new QTreeWidgetItem(ui.tree);
	pItemGroup->setText(0, tr("Перемещения"));
	pItemGroup->setFont(0, fnt);
	ui.tree->addTopLevelItem(pItemGroup);

	for(i=0; i<2; i++)
	{
		pItemRoot = new QTreeWidgetItem(pItemGroup);
		pItemRoot->setText(0, i == 0 ? tr("Зачисления") : tr("Списания"));
		pItemRoot->setData(0, Qt::UserRole, i == 0 ? 1 : -1);
		ui.tree->addTopLevelItem(pItemRoot);

		for(j=1; j<l_Intervals.size(); j++)
		{
			summa = 0;
			text = QString("SELECT val FROM operations WHERE type=%1 AND ur_person IN (%2) AND project IN (%3) AND date >= '%4' AND date < '%5' AND section IN (SELECT id FROM sections WHERE type=%6)")
				.arg(2)
				.arg(sUrPersons)
				.arg(sProjects)
				.arg(l_Intervals.at(j-1).toString("yyyy-MM-dd"))
				.arg(l_Intervals.at(j).toString("yyyy-MM-dd"))
				.arg(i);
			if (query2.exec(text))
			{
				while (query2.next()) 
				{
					summa += query2.value(0).toDouble();
				}
			}

#ifndef MONEY_FORMAT
			pItemRoot->setText(j, QString::number(summa, 'f', 2));
#else
			pItemRoot->setText(j, QString("%L1").arg(summa, 0, 'f', 2));
#endif
			pItemRoot->setTextAlignment(j, Qt::AlignRight);
		}
	}
	///////////////////////////////////////////////////////////////////

	QTreeWidgetItem *pItemItog = new QTreeWidgetItem(ui.tree);
	ui.tree->addTopLevelItem(pItemItog);
	pItemItog->setText(0, tr("Общий денежный поток"));
	pItemItog->setFont(0, fnt);
	
	QTreeWidgetItem *pItemDelta = new QTreeWidgetItem(ui.tree);
	ui.tree->addTopLevelItem(pItemDelta);
	pItemDelta->setText(0, tr("Остатки на конец периода"));
	pItemDelta->setFont(0, fnt);
	
	int n = ui.tree->topLevelItemCount();
	double summaAll = 0;
	for(i=1; i<l_Intervals.size(); i++)
	{
		summa = 0;

		for(j=0; j<n; j++)
		{
			summa += getSumma(ui.tree->topLevelItem(j), i);
		}

#ifndef MONEY_FORMAT
		pItemItog->setText(i, QString::number(summa, 'f', 2));
#else
		pItemItog->setText(i, QString("%L1").arg(summa, 0, 'f', 2));
#endif
		pItemItog->setFont(i, fnt);
		pItemItog->setTextAlignment(i, Qt::AlignRight);

		summaAll += summa;
#ifndef MONEY_FORMAT
		pItemDelta->setText(i, QString::number(summaAll, 'f', 2));
#else
		pItemDelta->setText(i, QString("%L1").arg(summaAll, 0, 'f', 2));
#endif
		pItemDelta->setFont(i, fnt);
		pItemDelta->setTextAlignment(i, Qt::AlignRight);

		ui.tree->resizeColumnToContents(i);
	}
	
	ui.tree->expandAll();
}
	
double ZProtokol::getSumma(QTreeWidgetItem *pItemRoot, int col)
{
	double s = 0;

	if(!pItemRoot)
		return s;

	int n = pItemRoot->childCount();
	for(int i=0; i<n; i++)
	{
		s += getSumma(pItemRoot->child(i), col);
	}

	if(n > 0)
	{
#ifndef MONEY_FORMAT
		pItemRoot->setText(col, QString::number(s, 'f', 2));
#else
		pItemRoot->setText(col, QString("%L1").arg(s, 0, 'f', 2));
#endif
		pItemRoot->setTextAlignment(col, Qt::AlignRight);
	}
	else
	{
#ifndef MONEY_FORMAT
		double v = pItemRoot->text(col).toDouble();
#else
		double v = pItemRoot->text(col).replace(QChar::Nbsp, "").toDouble();
#endif
		int k = pItemRoot->data(0, Qt::UserRole).toInt();
		if(k!=0)
			s += v * k;
	}
	return s;
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
	int i, j, k, l, n = ui.tree->topLevelItemCount();
	QTreeWidgetItem *pItemRoot, *pItem, *pItemChild;

	for(j=0; j<cols; j++)
	{
		out << ui.tree->headerItem()->text(j) << ";";
		if(j == 0)
			out << ";;";
	}
	out << "\n";

	for(i=0;i<n;i++)
	{
		pItemRoot = ui.tree->topLevelItem(i);
		for(j=0; j<cols; j++)
		{
			out << pItemRoot->text(j) << ";";
			if(j == 0)
				out << ";;";
		}
		out << "\n";

		for(k=0; k<pItemRoot->childCount(); k++)
		{
			pItem = pItemRoot->child(k);
			for(j=0; j<cols; j++)
			{
				if(j == 0)
					out << ";";
				out << pItem->text(j) << ";";
				if(j == 0)
					out << ";";
			}
			out << "\n";

			for(j=0; j<pItem->childCount(); j++)
			{
				pItemChild = pItem->child(j);
				for(l=0; l<cols; l++)
				{
					if(l == 0)
						out << ";;";
					out << pItemChild->text(l) << ";";
				}
				out << "\n";
			}
		}
	}

	return 1;
}
