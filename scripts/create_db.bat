@echo off
:
: Скрипт создания БД
:


:Windows

set create_script=plan.sql

set db_name=plan
set db_user=postgres
set db_host=127.0.0.1
set port=5432
set PGPASSWORD=postgres

: Проверка наличия БД
"c:\Program Files (x86)\PostgreSQL\9.4\bin\psql"  -c "select now()"  -At -h %db_host% -p %port% -U %db_user% --no-password -d %db_name% 

:если значение  errorlevel больше или равно  значению n (код возврата),
:то выполняется команда command

if errorlevel 2 goto dbNotExist 
if errorlevel 1  goto error
if errorlevel 0 goto dbExist
 
:dbNotExist
	echo База данных %db_name% отсутствует.
	goto createDb
	
:dbExist
	SET /P answer=База данных "%db_name%" существует. Удалить (y/n) ? 

	if "%answer%"=="n" goto finish

	echo Удаление базы данных %db_name% ...
    
    	"c:\Program Files (x86)\PostgreSQL\9.4\bin\dropdb" -h %db_host% -p %port% -U %db_user% --no-password %db_name%
    	
	if errorlevel 1 goto error 
	
	echo База данных %db_name% удалена.
	
:createDb	
: создание БД
echo Создание базы данных %db_name% ...
"c:\Program Files (x86)\PostgreSQL\9.4\bin\createdb" -h %db_host% -p %port% -U %db_user% --no-password %db_name%
if errorlevel 1 goto error

: загрузка данных в БД
echo Загрузка данных в БД %db_name% ...
"c:\Program Files (x86)\PostgreSQL\9.4\bin\psql" -h %db_host% -p %port% -U %db_user% --no-password -d %db_name% -f %create_script% 
if errorlevel 1 goto error
echo БД %db_name% успешно создана, исходные данные загружены.
:pause
exit 1

:finish
echo Остановка сценария создания БД.
:pause
exit 1

:error	
echo Ошибка при выполнении скрипта создания БД
pause
