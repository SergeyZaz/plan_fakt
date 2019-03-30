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
#include "zparsefile.h"

bool ZParseFile::loadFile(const QString &fileName)
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
"РасчСчет";		accounts:account

"СекцияДокумент",
  ----------------------"Номер",
  "Дата",	operations:date
  "Сумма",	operations:val
  "ПлательщикСчет", если ="РасчСчет", то это "Мой счет" (operations:type=1, table=ur_persons), иначе это "Контагент" (operations:type=0, table=partners)	
  ----------------------"ДатаСписано",
  "Плательщик",	table:name
  "ПлательщикИНН",	table:inn
  "ПлательщикКПП",	table:kpp
  ----------------------"ПлательщикРасчСчет", ="ПлательщикСчет"
  "ПлательщикБанк1",	accounts:bank
  "ПлательщикБИК",	accounts:bik
  ----------------------"ПлательщикКорсчет",	
  "ПолучательСчет", если ="РасчСчет", то это "Мой счет" (operations:type=0, table=ur_persons), иначе это "Контагент" (operations:type=1, table=partners)	
  ----------------------"ДатаПоступило",
  "Получатель",	table:name
  "ПолучательИНН",	table:inn
  "ПолучательКПП",	table:kpp
  ----------------------"ПолучательРасчСчет",	="ПлательщикСчет"
  "ПолучательБанк1",	accounts:bank
  "ПолучательБИК",	accounts:bik
  ----------------------"ПолучательКорсчет",	
  ----------------------"ВидПлатежа",
  ----------------------"ВидОплаты",
  ----------------------"СрокАкцепта",
  ----------------------"УсловиеОплаты1",
  ----------------------"СтатусСоставителя",
  ----------------------"ПоказательКБК",
  ----------------------"ОКАТО",
  ----------------------"ПоказательОснования",
  ----------------------"ПоказательПериода",
  ----------------------"ПоказательНомера",
  ----------------------"ПоказательДаты",
  ----------------------"ПоказательТипа",
  ----------------------"Очередность",
  ----------------------"Код",
  "НазначениеПлатежа",	operations:comment
  ----------------------"ВидАккредитива",
  ----------------------"СрокПлатежа",
  ----------------------"НомерСчетаПоставщика",
  ----------------------"ПлатежПоПредст",
  ----------------------"ДополнУсловия",
  ----------------------"ДатаОтсылкиДок",
  ----------------------"КонецДокумента"

*/

int ZParseFile::parseData(const QString &data)
{
	if(data.startsWith("СекцияДокумент"))
	{
		fParseStart = true;
	}

	if(!fParseStart)
	{
		if(data.startsWith("РасчСчет"))
		{
			QStringList items = data.split("=");
			if(!items[1].isEmpty())
				curSchet = items[1];
		}
		return 0;
	}

	if(data != "КонецДокумента")
	{
		QStringList items = data.split("=");
		if(!items[1].isEmpty())
			mapData.insert(items[0], items[1]);
		return 1;
	}
			
	fParseStart = false;

	QSqlQuery query;
	QString str_query, str_values, table[2], str_inn, prefix[2];
	
	table[0] = "ur_persons";
	table[1] = "partners";
	prefix[0] = "Получатель";
	prefix[1] = "Плательщик";
	
	bool fMinus = (mapData.value("ПлательщикСчет") == curSchet);// Я - плательщик!!!
	if(fMinus)
	{
		str_values = prefix[0];
		prefix[0] = prefix[1];
		prefix[1] = str_values;

		mapData.insert("Сумма", "-" + mapData.value("Сумма"));
	}
	

	int rc = 0, id = 0, idUrPerson = 0, idPartner = 0;
	///////////////////////	ur_persons / partners	///////////////////////
	for(int i=0; i<2; i++)
	{
		id = 0;
		str_inn = mapData.value(prefix[i] + "ИНН", " ");
		str_query = QString("SELECT id FROM %1 WHERE inn='%2'").arg(table[i]).arg(str_inn);
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
			str_query = QString("INSERT INTO %1 ( name,full_name,comment,inn,kpp ) VALUES ('%2', '%3', '%4', '%5', '%6')")
				.arg(table[i])
				.arg(mapData.value(prefix[i], " "))
				.arg(mapData.value(prefix[i], " "))
				.arg(" ")
				.arg(str_inn)
				.arg(mapData.value(prefix[i] + "КПП", " "));

			rc = query.exec(str_query);
			if(rc)
			{
				str_query = QString("SELECT id FROM %1 WHERE inn='%2'").arg(table[i]).arg(str_inn);
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

		if(i==0)
			idUrPerson = id;
		else
			idPartner = id;
	}
	///////////////////////	accounts	///////////////////////
	if(fMinus)
	{
		id = 0;
		str_query = QString("SELECT id FROM accounts WHERE account='%1'").arg(curSchet);
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
			str_query = QString("INSERT INTO accounts ( name,comment,type,ur_person,bik,bank,account ) VALUES ('%1', '%2', %3, %4, '%5', '%6', '%7')")
				.arg(mapData.value(prefix[0], " "))
				.arg(" ")
				.arg(1)		//Тип: 0-наличный/1-безналичный
				.arg(idUrPerson)
				.arg(mapData.value(prefix[0] + "БИК", " "))
				.arg(mapData.value(prefix[0] + "Банк1", " "))
				.arg(curSchet);

			rc = query.exec(str_query);
			if(rc)
			{
				str_query = QString("SELECT id FROM accounts WHERE account='%1'").arg(curSchet);
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
	}

	///////////////////////	operations	///////////////////////

	double val = mapData.value("Сумма", "0").toDouble();

	id = 0;
	str_query = QString("SELECT id FROM operations WHERE date='%1' AND type=%2 AND ur_person=%3 AND partner=%4 AND val=%5")
		.arg(mapData.value("Дата"))
		.arg(val < 0 ? 1 : 0)	//Тип: 0-Поступление/1-Выплата/2-Перемещение
		.arg(idUrPerson)
		.arg(idPartner)
		.arg(val);
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

	str_query = QString("INSERT INTO operations ( date,type,comment,ur_person,partner,val ) VALUES ('%1', %2, '%3', %4, %5, %6)")
		.arg(mapData.value("Дата"))
		.arg(val < 0 ? 1 : 0)	//Тип: 0-Поступление/1-Выплата/2-Перемещение
		.arg(mapData.value("НазначениеПлатежа", " "))
		.arg(idUrPerson)
		.arg(idPartner)
		.arg(val);

	mapData.clear();

	rc = query.exec(str_query);
    if(!rc)
    {
		message(query.lastError().text());
		return -1;
	}
	return 2;
}

void ZParseFile::message(const QString& txt)
{
	QString msg = txt;
	msg = QTextCodec::codecForName("UTF-8")->toUnicode(QTextCodec::codecForName("windows-1251")->fromUnicode(msg));
	QMessageBox::critical(NULL, QString("Ошибка"), msg);
	puts(txt.toStdString().c_str());
}

/*

int ZParseFile::parseData(const QString &data)
{
	if(data.startsWith("СекцияДокумент"))
	{
		fParseStart = true;
	}

	if(!fParseStart)
	{
		if(data.startsWith("РасчСчет"))
		{
			QStringList items = data.split("=");
			if(!items[1].isEmpty())
				curSchet = items[1];
		}
		return 0;
	}

	if(data != "КонецДокумента")
	{
		QStringList items = data.split("=");
		if(!items[1].isEmpty())
			mapData.insert(items[0], items[1]);
		return 1;
	}
			
	if(mapData.value("ПлательщикСчет") == curSchet)
		mapData.insert("Сумма", "-" + mapData.value("Сумма"));

	fParseStart = false;
	QString str_query = "INSERT INTO from_file(";
	QString str_values = "VALUES(";
	QMap<QString, QString>::const_iterator iT = mapData.constBegin();
	while (iT != mapData.constEnd()) 
	{
		str_query += "\"" + iT.key() + "\",";
		str_values += "'" + iT.value() + "',";
		++iT;
	}
	str_query.chop(1);
	str_values.chop(1);
	str_query += ") " + str_values + ");";
	mapData.clear();

	QSqlQuery query;
	int rc = query.exec(str_query);
    if(!rc)
    {
		QString msg = query.lastError().text();
		msg = QTextCodec::codecForName("UTF-8")->toUnicode(QTextCodec::codecForName("windows-1251")->fromUnicode(msg));
		QMessageBox::critical(NULL, QString("Ошибка"), msg);
		puts(query.lastError().text().toStdString().c_str());
		return -1;
	}
	return 2;
}

*/