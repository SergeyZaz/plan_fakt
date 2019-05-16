#include <QString>
#include <QSqlQuery>
#include <QSqlError>
#include <QStringList>
#include <QMessageBox>
#include <QTextCodec>
#include <QFile>
#include <QTextStream>
#include <QApplication>
#include <QVariant>
#include <QProgressDialog>
#include "zparsecsvfile.h"

bool ZParseCSVFile::loadFile(const QString &fileName)
{
	QFile file(fileName);
	if (!file.open(QFile::ReadOnly | QFile::Text)) 
	{
		QMessageBox::warning(NULL, QString("Ошибка"),
			QString("Невозможно открыть файл %1:\n%2.")
			.arg(fileName)
			.arg(file.errorString()));
		return false;
	}

	int count = 1000;
	QProgressDialog progress("Обработка файла...", "Отмена", 0, count, QApplication::activeWindow());
	progress.setWindowModality(Qt::WindowModal);
	progress.show();

	QTextStream in(&file);
	QApplication::setOverrideCursor(Qt::WaitCursor);
	
	QString line = in.readLine();
	while (!line.isNull()) 
	{
		if (progress.wasCanceled())
             break;
		parseData(line);
		line = in.readLine();
	}
	
	progress.setValue(count);

	QApplication::restoreOverrideCursor();
	return true;
}

//*****************************************************************************************
/*
************************************************************
0 Дата оплаты;
1 Статус оплаты;
2 Дата начисления;
3 Статус начисления;
4 Контрагент;
5 ИНН контрагента;
6 Тип;
7 Счет;
8 № Счета;
9 Банк;
10 Бик;
11 Юрлицо;
12 ИНН юрлица;
13 Статья;
14 Родительские статьи;
15 Комментарий;
!16 Проекты;
17 Сумма;
18 Валюта
************************************************************
0 30.06.2017;
1 Подтверждена;
2 ;
3 ;
4 Филиппова Анастасия;
5 ;
6 Выплата;
7 Карта_Альфа_Анастасия;
8 ;
9 ;
10 ;
11 ИП Филиппова Анастас;
12 ;
13 РКО;
14 Прочие расходы;
15 ;
16 1 (100%);
17 199,00;
18 RUB
************************************************************
*/

int ZParseCSVFile::parseData(const QString &data)
{
	if(data.startsWith("Дата"))
		return 0;

	QStringList items = data.split(";");
	if(items.size() != 19)
		return 0;

	QSqlQuery query;
	QString str_query, str_values, table[2];
	
	table[1] = "ur_persons";
	table[0] = "partners";
	
	bool fMinus = (items[6]==QString("Выплата"));// Я - плательщик!!!

	int rc = 0, id = 0, idUrPerson = 0, idPartner = 0, section_plus = 0, section_minus = -1, idAccount = 0;
	///////////////////////	ur_persons / partners	///////////////////////
	for(int i=0; i<2; i++)
	{
		id = 0;
		str_query = QString("SELECT id FROM %1 WHERE inn='%2'").arg(table[i]).arg(items[5+7*i]);
		rc = query.exec(str_query);
		if(rc)
		{
			while (query.next()) 
			{
				id = query.value(0).toInt();
				break;
			}
		}
		else
		{
			message(query.lastError().text());
		}

		if(id == 0)
		{
			str_query = QString("INSERT INTO %1 ( name,full_name,comment,inn,kpp ) VALUES ('%2', '%2', ' ', '%3', ' ')")
				.arg(table[i])
				.arg(items[4+7*i])
				.arg(items[5+7*i]);

			rc = query.exec(str_query);
			if(rc)
			{
				str_query = QString("SELECT id FROM %1 WHERE inn='%2'").arg(table[i]).arg(items[5+7*i]);
				rc = query.exec(str_query);
				if(rc)
				{
					while (query.next()) 
					{
						id = query.value(0).toInt();
						break;
					}
				}
				else
				{
					message(query.lastError().text());
				}
			}
			else
			{
				message(query.lastError().text());
			}
		}
		else
		{
			if(i==0)
			{
				str_query = QString("SELECT section_plus,section_minus FROM %1 WHERE id='%2'").arg(table[i]).arg(id);
				rc = query.exec(str_query);
				if(rc)
				{
					while (query.next()) 
					{
						section_plus = query.value(0).toInt();
						section_minus = query.value(1).toInt();
						break;
					}
				}
				else
				{
					message(query.lastError().text());
				}
			}
		}

		if(i==1)
			idUrPerson = id;
		else
			idPartner = id;
	}

	///////////////////////	sections	///////////////////////
	id = 0;
	str_query = QString("SELECT id FROM sections WHERE name='%1' AND type=%2") // 0-Доходы/1-Расходы
		.arg(items[13])
		.arg(fMinus);
	rc = query.exec(str_query);
	if(rc)
	{
		while (query.next()) 
		{
			id = query.value(0).toInt();
			break;
		}
	}
	else
	{
		message(query.lastError().text());
		return 0;
	}

	if(id == 0)
	{
		str_query = QString("INSERT INTO sections ( name,type,parent ) VALUES ('%1', '%2', 0)")
			.arg(items[13])
			.arg(fMinus);

		rc = query.exec(str_query);
		if(rc)
		{
			str_query = QString("SELECT id FROM sections WHERE name='%1' AND type=%2") // 0-Доходы/1-Расходы
				.arg(items[13])
				.arg(fMinus);
			rc = query.exec(str_query);
			if(rc)
			{
				while (query.next()) 
				{
					id = query.value(0).toInt();
					break;
				}
			}
			else
			{
				message(query.lastError().text());
			}
		}
		else
		{
			message(query.lastError().text());
		}
	}
	if(id)
	{
		if(fMinus)
			section_minus = id;
		else
			section_plus = id;
	}


	///////////////////////	accounts	///////////////////////
	idAccount = 0;
	str_query = QString("SELECT id FROM accounts WHERE account='%1'").arg(items[8]);
	rc = query.exec(str_query);
	if(rc)
	{
		while (query.next()) 
		{
			idAccount = query.value(0).toInt();
			break;
		}
	}
	else
	{
		message(query.lastError().text());
	}

	if(idAccount == 0)
	{
		str_query = QString("INSERT INTO accounts ( name,comment,type,ur_person,bik,bank,account ) VALUES ('%1', '%2', %3, %4, '%5', '%6', '%7')")
			.arg(items[7])
			.arg(" ")
			.arg(1)		//Тип: 0-наличный/1-безналичный
			.arg(idUrPerson)
			.arg(items[10])
			.arg(items[9])
			.arg(items[8]);

		rc = query.exec(str_query);
		if(rc)
		{
			str_query = QString("SELECT id FROM accounts WHERE account='%1'").arg(items[8]);
			rc = query.exec(str_query);
			if(rc)
			{
				while (query.next()) 
				{
					idAccount = query.value(0).toInt();
					break;
				}
			}
			else
			{
				message(query.lastError().text());
			}
		}
		else
		{
			message(query.lastError().text());
		}
	}


	///////////////////////	operations	///////////////////////

	double val = items[17].toDouble();

	id = 0;
	str_query = QString("SELECT id FROM operations WHERE date='%1' AND type=%2 AND ur_person=%3 AND partner=%4 AND val=%5 AND account=%6")
		.arg(fMinus ? items[0] : items[2])
		.arg(fMinus ? 1 : 0) 	//Тип: 0-Поступление/1-Выплата/2-Перемещение
		.arg(idUrPerson)
		.arg(idPartner)
		.arg(val)
		.arg(idAccount);
	rc = query.exec(str_query);
	if(rc)
	{
		while (query.next()) 
		{
			id = query.value(0).toInt();
			break;
		}
	}
	else
	{
		message(query.lastError().text());
	}

	if(id > 0)
		return 0;

	str_query = QString("INSERT INTO operations ( date,type,comment,ur_person,partner,val,section,account ) VALUES ('%1', %2, '%3', %4, %5, %6, %7, %8)")
		.arg(fMinus ? items[0] : items[2])
		.arg(fMinus ? 1 : 0) //(val < 0 )	//Тип: 0-Поступление/1-Выплата/2-Перемещение
		.arg(items[15])
		.arg(idUrPerson)
		.arg(idPartner)
		.arg(val)
		.arg(fMinus ? section_minus : section_plus)
		.arg(idAccount);

	rc = query.exec(str_query);
    if(!rc)
    {
		message(query.lastError().text());
		return -1;
	}
	return 2;
}

void ZParseCSVFile::message(const QString& txt)
{
	QMessageBox::critical(NULL, QString("Ошибка"), txt);
	puts(txt.toStdString().c_str());
}

