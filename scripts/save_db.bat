set db_name=plan
set db_user="postgres"
set db_host=127.0.0.1
set port=5432
set PGPASSWORD=postgres


IF "%time:~0,1%" LSS "1" (
   SET DATETIME=%date:~6,4%.%date:~3,2%.%date:~0,2%-0%time:~1,1%.%time:~3,2%.%time:~6,2%
) ELSE (
   SET DATETIME=%date:~6,4%.%date:~3,2%.%date:~0,2%-%time:~0,2%.%time:~3,2%.%time:~6,2%
)

"c:\Program Files (x86)\PostgreSQL\9.4\bin\pg_dump" --host %db_host% --port %port% --username %db_user% --no-password --format plain --inserts --verbose --file %DATETIME%_%db_name%.sql %db_name% 



rem pause