/*
 * sqlite.h
 *
 *  Created on: Jan 31, 2023
 *      Author: user
 */

#ifndef TOOLS_SQLITE_HELPER_H_
#define TOOLS_SQLITE_HELPER_H_

#include <hda_service.h>
#include <sqlite3.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <stdlib.h>
#include <storage.h>
#include <app_common.h>
#include <stdio.h>

//typedef struct
//{
//    int id;
//    char msg[MAX_LEN];
//    char date[MAX_LEN];
//} QueryData;
//
//int sqlite_helper_check_useable_and_open(sqlite3 *db_h, char* db_name);
//int sqlite_helper_init_db(sqlite3 *db_h, char* db_name);
//int sqlite_helper_insert(sqlite3 *db_h, const char * msg_data);


/*this structure will be commonly used in both database and application layer*/

#define MAX_LEN 500
typedef struct
{
    int id;
    int type;
    char msg[MAX_LEN];
    char date[MAX_LEN];
} QueryData;

int opendb();
int initdb();

/*inset type, msg in the database. Date will be stored from system and id is autoincrement*/
int insertMsgIntoDb(int type, const char * msg_data);

/*fetch all stored message form database. This API will return total number of rows found in this call*/
int getAllMsgFromDb(QueryData **msg_data, int* num_of_rows);

/*fetch stored message form database based on given ID. Application needs to send desired ID*/
int getMsgById(QueryData **msg_data, int id);

/*delete stored message form database based on given ID. Application needs to send desired ID*/
int deleteMsgById(int id);

/*fetch all stored message form database*/
int deleteMsgAll();

/*count number of stored msg in the database and will return the total number*/
int getTotalMsgItemsCount(int* num_of_rows);

char* get_write_filepath(char *filename);
char* write_file(char* filepath, char* buf);
char* append_file(char* filepath, char* buf);
char* read_file(char* filepath);

static bool storage_cb(int storage_id, storage_type_e type, storage_state_e state, const char *path, void *user_data);


#endif /* TOOLS_SQLITE_HELPER_H_ */
