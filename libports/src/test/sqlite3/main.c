#include <stdio.h>
#include <sqlite3.h>

static int callback(void *pArg, int argc, char **argv, char **columnNames){
	int i;
	for(i=0; i < argc; i++){
		printf("%s = %s\n", columnNames[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");
	return 0;
}

int main(int argc, char **argv){
	sqlite3     *db;
	char        *err = 0;
	int         rc;
	int         i;
	
	const char *sql[] = {
		"create table test_table (text1 varchar(30), int1 smallint)",
		"insert into  test_table (text1, int1) values ('test1', 1)",
		"insert into  test_table (text1, int1) values ('test2', 2)",
		"insert into  test_table (text1, int1) values ('test3', 3)",
		"select * from test_table"
	};

	rc = sqlite3_open(":memory:", &db);
	if( rc ){
		printf("Can't open database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return 1;
	}
	
	for (i = 0; i < sizeof(sql)/sizeof(sql[0]); ++i)
	{
		rc = sqlite3_exec(db, sql[i], callback, 0, &err);
		if( rc != SQLITE_OK ){
			printf("SQL error: %s\n", err);
			sqlite3_free(err);
		}
	}
	sqlite3_close(db);
	return 0;
}