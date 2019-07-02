@echo off
:
: ��ਯ� ᮧ����� ��
:


:Windows

set create_script=plan.sql

set db_name=plan
set db_user=postgres
set db_host=127.0.0.1
set port=5432
set PGPASSWORD=postgres

: �஢�ઠ ������ ��
"c:\Program Files (x86)\PostgreSQL\9.4\bin\psql"  -c "select now()"  -At -h %db_host% -p %port% -U %db_user% --no-password -d %db_name% 

:�᫨ ���祭��  errorlevel ����� ��� ࠢ��  ���祭�� n (��� ������),
:� �믮������ ������� command

if errorlevel 2 goto dbNotExist 
if errorlevel 1  goto error
if errorlevel 0 goto dbExist
 
:dbNotExist
	echo ���� ������ %db_name% ���������.
	goto createDb
	
:dbExist
	SET /P answer=���� ������ "%db_name%" �������. ������� (y/n) ? 

	if "%answer%"=="n" goto finish

	echo �������� ���� ������ %db_name% ...
    
    	"c:\Program Files (x86)\PostgreSQL\9.4\bin\dropdb" -h %db_host% -p %port% -U %db_user% --no-password %db_name%
    	
	if errorlevel 1 goto error 
	
	echo ���� ������ %db_name% 㤠����.
	
:createDb	
: ᮧ����� ��
echo �������� ���� ������ %db_name% ...
"c:\Program Files (x86)\PostgreSQL\9.4\bin\createdb" -h %db_host% -p %port% -U %db_user% --no-password %db_name%
if errorlevel 1 goto error

: ����㧪� ������ � ��
echo ����㧪� ������ � �� %db_name% ...
"c:\Program Files (x86)\PostgreSQL\9.4\bin\psql" -h %db_host% -p %port% -U %db_user% --no-password -d %db_name% -f %create_script% 
if errorlevel 1 goto error
echo �� %db_name% �ᯥ譮 ᮧ����, ��室�� ����� ����㦥��.
:pause
exit 1

:finish
echo ��⠭���� �業��� ᮧ����� ��.
:pause
exit 1

:error	
echo �訡�� �� �믮������ �ਯ� ᮧ����� ��
pause
