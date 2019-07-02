@echo off
:
: Скрипт обновления БД
:


:Windows

set create_script=plan_update.sql

set db_name=plan
set db_user=postgres
set db_host=127.0.0.1
set port=5432
set PGPASSWORD=postgres


: загрузка данных в БД
echo Загрузка данных в БД %db_name% ...
"c:\Program Files (x86)\PostgreSQL\9.4\bin\psql" -h %db_host% -p %port% -U %db_user% --no-password -d %db_name% -f %create_script% 
if errorlevel 1 goto error
echo БД %db_name%  данные загружены.
pause
exit 1


:error	
echo Ошибка при выполнении скрипта создания БД
pause
