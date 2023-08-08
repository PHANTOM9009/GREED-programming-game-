#include <stdio.h>
#include <stdlib.h>
#include<iostream>
#include "sqlite3.h" 

using namespace std;
static int callback(void* NotUsed, int argc, char** argv, char** azColName) 
{
    /*
    * this thing is running for every row, argc is the number of columns in the database,
    * argv is the value at that column
    * azColName is the name of the column
    */
    int i;
    
    for (i = 0; i < argc; i++) 
    {
        cout << azColName[i] << " =>" << argv[i] << endl;
    }
    printf("\n");
    return 0;
}

int main(int argc, char* argv[]) 
{
    sqlite3* db;
    char* zErrMsg = 0;
    int rc;
    

    /* Open database */
    rc = sqlite3_open("Greed.db", &db);

    if (rc)
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return(0);
    }
    else 
    {
        fprintf(stdout, "Opened database successfully\n");
    }

    /* Create SQL statement */
    /*
    char sql[] = "CREATE TABLE USER_DATA("\
        "ID INT PRIMARY KEY NOT NULL,"\
        "USERNAME TEXT NOT NULL,"\
        "PASSWORD TEXT NOT NULL," \
        "EMAIL TEXT NOT NULL," \
        "ROLE TEXT NOT NULL,"\
        "COUNTRY TEXT NOT NULL);";
     */
    //adding some dummy values in the database
    /*
    char sql[] ="INSERT INTO USER_DATA(ID,USERNAME,PASSWORD,EMAIL,ROLE,COUNTRY)"\
        "VALUES(2,'user','password','abc1@gmail.com',0,'India');"\
        "INSERT INTO USER_DATA(ID,USERNAME,PASSWORD,EMAIL,ROLE,COUNTRY)"\
        "VALUES(3,'user1','password','abc2@gmail.com',0,'India');"\
        "INSERT INTO USER_DATA(ID,USERNAME,PASSWORD,EMAIL,ROLE,COUNTRY)"\
        "VALUES(4,'user2','password','abc3@gmail.com',0,'India');"\
        "INSERT INTO USER_DATA(ID,USERNAME,PASSWORD,EMAIL,ROLE,COUNTRY)"\
        "VALUES(5,'user3','password','abc4@gmail.com',0,'India');"\
        "INSERT INTO USER_DATA(ID,USERNAME,PASSWORD,EMAIL,ROLE,COUNTRY)"\
        "VALUES(6,'user4','password','abc5@gmail.com',0,'India');"\
        "INSERT INTO USER_DATA(ID,USERNAME,PASSWORD,EMAIL,ROLE,COUNTRY)"\
        "VALUES(7,'user5','password','abc6@gmail.com',0,'India');"\
        "INSERT INTO USER_DATA(ID,USERNAME,PASSWORD,EMAIL,ROLE,COUNTRY)"\
        "VALUES(8,'user6','password','abc7@gmail.com',0,'India');";
    */

    
    char sql[] = "select * from user_data where id==9;";
    char sql1[] = "CREATE TABLE METADATA("\
        "ID INT,"\
        "GAMES_PLAYED INT,"\
        "TOTAL_SCORE INT,"\
        "RANK INT,"\
        "FOREIGN KEY (ID) REFERENCES USER_DATA);";
    char sql2[] = "UPDATE USER_DATA SET USERNAME='username';";
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