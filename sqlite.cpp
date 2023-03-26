#include <stdio.h>
#include <stdlib.h>
#include<iostream>
#include "sqlite3.h" 
using namespace std;
static int callback(void* NotUsed, int argc, char** argv, char** azColName) {
    int i;
    cout << "\nargc=>" << argc;
    for (i = 0; i < argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

int main(int argc, char* argv[]) {
    sqlite3* db;
    char* zErrMsg = 0;
    int rc;
    

    /* Open database */
    rc = sqlite3_open("test1.db", &db);

    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return(0);
    }
    else {
        fprintf(stdout, "Opened database successfully\n");
    }

    /* Create SQL statement */
    char sql[] = "SELECT * from COMPANY \
        where ID==4 and NAME='Mark'";

    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql, callback,0, &zErrMsg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    else {
        fprintf(stdout, "Operation done successfully\n");
    }
    cout << "\n hi dear";

    sqlite3_close(db);
    return 0;
}