@echo off
:
: ��ਯ� ���������� ��
:


:Windows

set create_script=plan_update.sql

set db_name=plan
set db_user=postgres
set db_host=127.0.0.1
set port=5432
set PGPASSWORD=postgres


: ����㧪� ������ � ��
echo ����㧪� ������ � �� %db_name% ...
"c:\Program Files (x86)\PostgreSQL\9.4\bin\psql" -h %db_host% -p %port% -U %db_user% --no-password -d %db_name% -f %create_script% 
if errorlevel 1 goto error
echo �� %db_name%  ����� ����㦥��.
pause
exit 1


:error	
echo �訡�� �� �믮������ �ਯ� ᮧ����� ��
pause
