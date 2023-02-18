#include <tools/sqlite_helper.h>

#define DB_NAME "sample.db"
#define TABLE_NAME "TizenSensorTable"
#define COL_ID "ID"
#define COL_DATA "DATA"
#define COL_TYPE "CODE"
#define COL_DATE "DATE"
#define BUFLEN 500


////sqlite3 *db; /* Database handle */
//
//void sqlite3_manager_initialize(){
//	sqlite3_shutdown();
//	slqite3_config(SQLITE_CONFIG_URI, 1);
//
//	sqlite3_initialize();
//}
//
//void sqlite3_manager_open(char *db_name, sqlite3 *db_h){
//	char file_path[BUFLEN];
//	char *document_path;
//	/*
//	   Fill the variable with the value obtained
//	   using storage_foreach_device_supported()
//	*/
//	int internal_storage_id = 0;
//	storage_get_directory(internal_storage_id, STORAGE_DIRECTORY_DOCUMENTS, &document_path);
//
//	dlog_print(DLOG_DEBUG, SQLITE3_LOG_TAG, "Database path:: %s\\%s", document_path, db_name);
//	free(document_path);
//	sqlite3_open(file_path, &db_h);
//	free(file_path);
//}
//
//static int CreateTable(sqlite3 *db_h)
//{
//    int ret;
//    char *ErrMsg;
//    char *sql = "CREATE TABLE IF NOT EXISTS EncryptedData("  \
//                                                          "DATA TEXT NOT NULL," \
//                                                          "ENCRYPTED INT NOT NULL,"\
//                                                          "SALT TEXT NOT NULL," \
//                                                          "IV TEXT NOT NULL," \
//                                                          "PART INTEGER," \
//                                                           "KEY INTEGER PRIMARY KEY);";
//
//    sqlite3_exec(db_h, sql, callback, 0, &ErrMsg);
//    return 0;
//}
//
//static int InsertRecord(sqlite3 *db_h, unsigned char *msg, int encrypted, int part, int len)
//{
//    char *ErrMsg;
//    ret = sqlite3_exec(db_h, msg, callback, 0, &ErrMsg);
//    if (ret) {
//        dlog_print(DLOG_ERROR, SQLITE3_LOG_TAG, "Error: %s\n", ErrMsg);
//        sqlite3_free(ErrMsg);
//    }
//
//    return 0;
//}

//int sqlite_helper_check_useable_and_open(sqlite3 *db_h, char* db_name){
//	char *temp = app_get_data_path();
//	dlog_print(DLOG_INFO, SQLITE3_LOG_TAG, "DataPath = [%s]", temp); /*prepared full path, database will be stored there*/
//
//	int size = strlen(temp) + 10;
//	char * document_path = malloc(sizeof(char) * size);
//
//	strcpy(document_path, temp);
//	strncat(document_path, db_name, size);
//	dlog_print(DLOG_INFO, SQLITE3_LOG_TAG, "Database path: [%s]", document_path);
//
//	int rc = sqlite3_open(document_path, &db_h);
//	dlog_print(DLOG_ERROR, SQLITE3_LOG_TAG, "rc: [%i]", rc);
//
//	if(rc != SQLITE_OK){
//		dlog_print(DLOG_ERROR, SQLITE3_LOG_TAG, "Cannot open database: %s [%s]", sqlite3_errmsg(db_h), document_path);
//		sqlite3_close(db_h);
//	}
//	dlog_print(DLOG_DEBUG, SQLITE3_LOG_TAG, "Database is opened");
//	return rc;
//}
//
//int sqlite_helper_init_db(sqlite3 *db_h, char* db_name){
//	if(sqlite_helper_check_useable_and_open(db_h, db_name) != SQLITE_OK){
//		return SQLITE_ERROR;
//	}
//
//	int ret;
//	char *ErrMsg;
//
//	char *sql = "CREATE TABLE IF NOT EXISTS "\
//		    		TABLE_NAME" ("  \
//					COL_DATA" TEXT NOT NULL, " \
//					COL_DATE" TEXT NOT NULL, " \
//					COL_ID" INTEGER PRIMARY KEY AUTOINCREMENT);";
//	dlog_print(DLOG_DEBUG, SQLITE3_LOG_TAG, "Create table query : %s", sql);
//
//	ret = sqlite3_exec(db_h, sql, NULL, 0, &ErrMsg);
//	if(ret != SQLITE_OK){
//		dlog_print(DLOG_ERROR, SQLITE3_LOG_TAG, "Table Create Error! [%s]", ErrMsg);
//		sqlite3_free(ErrMsg);
//		sqlite3_close(db_h);
//
//		return SQLITE_ERROR;
//	}
//
//	dlog_print(DLOG_DEBUG, SQLITE3_LOG_TAG, "DB Table created successfully!");
//	sqlite3_close(db_h);
//
//	return SQLITE_OK;
//}
//
///*callback for insert operation*/
//static int insertcb(void *NotUsed, int argc, char **argv, char **azColName){
//   int i;
//   for(i=0; i<argc; i++){
//      /*usually we do not need to do anything.*/
//   }
//   return 0;
//}
//
//int sqlite_helper_insert(sqlite3 *db_h, char* db_name, const char * msg_data){
//	if(sqlite_helper_check_useable_and_open(db_h, db_name) != SQLITE_OK){
//		return SQLITE_ERROR;
//	}
//
//	char sqlbuff[BUFLEN];
//	char *ErrMsg;
//	int ret;
//
//	char * dateTime = "strftime('%Y-%m-%d  %H-%M','now')";
//	snprintf(sqlbuff, BUFLEN, "INSERT INTO "\
//				TABLE_NAME" VALUES(\'%s\', %s, NULL);", /*didn't include id as it is autoincrement*/
//		            		msg_data, dateTime);
//	dlog_print(DLOG_DEBUG, SQLITE3_LOG_TAG, "Insert query = [%s]", sqlbuff);
//
//	ret = sqlite3_exec(db_h, sqlbuff, insertcb, 0, &ErrMsg);
//	if(ret != SQLITE_OK){
//		dlog_print(DLOG_ERROR, SQLITE3_LOG_TAG, "Insertion Error! [%s]", sqlite3_errmsg(sampleDb));
//		sqlite3_free(ErrMsg);
//		sqlite3_blob_close(db_h);
//
//		return SQLITE_ERROR;
//	}
//	sqlite3_close(db_h);
//
//	return SQLITE_OK;
//}
//
//QueryData *qrydata;

sqlite3 *sampleDb; /*name of database*/
int select_row_count = 0;

/*open database instance*/
int opendb()
{
     char * dataPath = app_get_data_path(); /*fetched package path available physically in the device*/
     dlog_print(DLOG_INFO, SQLITE3_LOG_TAG, "Data path = [%s]", dataPath); /*prepared full path, database will be stored there*/

	 int size = strlen(dataPath)+10;
	 char * path = malloc(sizeof(char)*size);

	 strcpy(path, dataPath);
	 strncat(path, DB_NAME, size);
	 dlog_print(DLOG_INFO, SQLITE3_LOG_TAG, "DB Path = [%s]", path); /*prepared full path, database will be stored there*/

	 int ret = sqlite3_open(path , &sampleDb);
	 if(ret != SQLITE_OK)
		 dlog_print(DLOG_ERROR, SQLITE3_LOG_TAG, sqlite3_errmsg(sampleDb));

	 free(dataPath);
	 free(path);
     /*didn't close database instance as this will be handled by caller e.g. insert, delete*/
	 return ret;
}

int initdb()
{
	if (opendb() != SQLITE_OK) /*create database instance*/
		return SQLITE_ERROR;

   int ret;
   char *ErrMsg;
   /*query preparation for table creation. it will not be created the table if it is exists already*/
   char *sql = "CREATE TABLE IF NOT EXISTS "\
		    TABLE_NAME" ("  \
			COL_DATA" TEXT NOT NULL, " \
			COL_TYPE" INTEGER NOT NULL, " \
			COL_DATE" TEXT NOT NULL, " \
			COL_ID" INTEGER PRIMARY KEY AUTOINCREMENT);"; /*id autoincrement*/

   dlog_print(DLOG_INFO, SQLITE3_LOG_TAG,"crate table query : %s", sql);

   ret = sqlite3_exec(sampleDb, sql, NULL, 0, &ErrMsg); /*execute query*/
   if(ret != SQLITE_OK)
   {
	   dlog_print(DLOG_ERROR, SQLITE3_LOG_TAG,"Table Create Error! [%s]", ErrMsg);
	   sqlite3_free(ErrMsg);
	   sqlite3_close(sampleDb); /*close db instance as instance is still open*/

	   return SQLITE_ERROR;
   }
   dlog_print(DLOG_INFO, SQLITE3_LOG_TAG,"Db Table created successfully!");
   sqlite3_close(sampleDb); /*close the db instance as operation is done here*/

   return SQLITE_OK;
}

/*callback for insert operation*/
static int insertcb(void *NotUsed, int argc, char **argv, char **azColName){
   int i;
   for(i=0; i<argc; i++){
      /*usually we do not need to do anything.*/
   }
   return 0;
}

int insertMsgIntoDb(int type, const char * msg_data)
{
	if(opendb() != SQLITE_OK) /*create database instance*/
		return SQLITE_ERROR;

	char sqlbuff[BUFLEN];
	char *ErrMsg;
	int ret;
	/*read system date time using sqlite function*/
	char* dateTime = "strftime('%Y-%m-%d  %H-%M','now')";

        /*prepare query for INSERT operation*/
	snprintf(sqlbuff, BUFLEN, "INSERT INTO "\
			TABLE_NAME" VALUES(\'%s\', %d, %s, NULL);", /*didn't include id as it is autoincrement*/
	            		msg_data, type, dateTime);
	dlog_print(DLOG_INFO, SQLITE3_LOG_TAG,"Insert query = [%s]", sqlbuff);

	ret = sqlite3_exec(sampleDb, sqlbuff, insertcb, 0, &ErrMsg); /*execute query*/
	if (ret != SQLITE_OK)
	{
		dlog_print(DLOG_ERROR, SQLITE3_LOG_TAG,"Insertion Error! [%s]", sqlite3_errmsg(sampleDb));
	   sqlite3_free(ErrMsg);
	   sqlite3_close(sampleDb); /*close db instance for failed case*/

	   return SQLITE_ERROR;
	}

	sqlite3_close(sampleDb); /*close db instance for success case*/

	return SQLITE_OK;
}

QueryData *qrydata;

/*this callback will be called for each row fetched from database. we need to handle retrieved elements for each row manually and store data for further use*/
static int selectAllItemcb(void *data, int argc, char **argv, char **azColName){
        /*
        * SQLite queries return data in argv parameter as  character pointer */
        /*prepare a temporary structure*/
	QueryData *temp = (QueryData*)realloc(qrydata, ((select_row_count + 1) * sizeof(QueryData)));

	if(temp == NULL){
		dlog_print(DLOG_ERROR, SQLITE3_LOG_TAG, "Cannot reallocate memory for QueryData");
		return SQLITE_ERROR;
	} else {
                /*store data into temp structure*/
		strcpy(temp[select_row_count].msg, argv[0]);
		temp[select_row_count].type = atoi(argv[1]);
		strcpy(temp[select_row_count].date, argv[2]);
		temp[select_row_count].id = atoi(argv[3]);

                /*copy temp structure into main sturct*/
		qrydata = temp;
	}

	select_row_count ++; /*keep row count*/

   return SQLITE_OK;
}

int getAllMsgFromDb(QueryData **msg_data, int* num_of_rows)
{
	if(opendb() != SQLITE_OK) /*create database instance*/
		return SQLITE_ERROR;

	qrydata = (QueryData *) calloc (1, sizeof(QueryData)); /*preparing local querydata struct*/

   char *sql = "SELECT * FROM "\
		   TABLE_NAME" ORDER BY ID DESC"; /*select query*/
   int ret;
   char *ErrMsg;
   select_row_count = 0;

    ret = sqlite3_exec(sampleDb, sql, selectAllItemcb, (void*)msg_data, &ErrMsg);
	if (ret != SQLITE_OK)
	{
	dlog_print(DLOG_ERROR, SQLITE3_LOG_TAG,"select query execution error [%s]", ErrMsg);
	   sqlite3_free(ErrMsg);
	   sqlite3_close(sampleDb); /*close db for failed case*/

	   return SQLITE_ERROR;
	}

        /*assign all retrived values into caller's pointer*/
	*msg_data = qrydata;
        *num_of_rows = select_row_count;

        dlog_print(DLOG_INFO, SQLITE3_LOG_TAG, "select query execution success!");
	sqlite3_close(sampleDb); /*close db for success case*/

   return SQLITE_OK;
}

int getMsgById(QueryData **msg_data, int id)
{
	if(opendb() != SQLITE_OK) /*create database instance*/
		return SQLITE_ERROR;

	qrydata = (QueryData *) calloc (1, sizeof(QueryData));

   char sql[BUFLEN];
   snprintf(sql, BUFLEN, "SELECT * FROM TizenSensorTable where ID=%d;", id);

   int ret = 0;
   char *ErrMsg;

    ret = sqlite3_exec(sampleDb, sql, selectAllItemcb, (void*)msg_data, &ErrMsg);
	if (ret != SQLITE_OK)
	{
		dlog_print(DLOG_ERROR, SQLITE3_LOG_TAG,"select query execution error [%s]", ErrMsg);
	   sqlite3_free(ErrMsg);
	   sqlite3_close(sampleDb);

	   return SQLITE_ERROR;
	}

	dlog_print(DLOG_INFO, SQLITE3_LOG_TAG, "select query execution success!");

        /*assign fetched data into caller's struct*/
	*msg_data = qrydata;

	sqlite3_close(sampleDb); /*close db*/

   return SQLITE_OK;
}

static int deletecb(void *data, int argc, char **argv, char **azColName){
   int i;
   for(i=0; i<argc; i++){
	/*no need to do anything*/
   }

   return 0;
}

int deleteMsgById(int id)
{
	if(opendb() != SQLITE_OK) /*create database instance*/
		return SQLITE_ERROR;

   char sql[BUFLEN];
   snprintf(sql, BUFLEN, "DELETE from TizenSensorTable where ID=%d;", id);

   int counter = 0, ret = 0;
   char *ErrMsg;

   ret = sqlite3_exec(sampleDb, sql, deletecb, &counter, &ErrMsg);
	if (ret != SQLITE_OK)
	{
		dlog_print(DLOG_ERROR, SQLITE3_LOG_TAG,"Delete Error! [%s]", sqlite3_errmsg(sampleDb));
	   sqlite3_free(ErrMsg);
	   sqlite3_close(sampleDb);

	   return SQLITE_ERROR;
	}

	sqlite3_close(sampleDb);

   return SQLITE_OK;
}

int deleteMsgAll()
{
	if(opendb() != SQLITE_OK) /*create database instance*/
		return SQLITE_ERROR;

   char sql[BUFLEN];
   snprintf(sql, BUFLEN, "DELETE from QueryData;");

   int counter = 0, ret = 0;
   char *ErrMsg;

   ret = sqlite3_exec(sampleDb, sql, deletecb, &counter, &ErrMsg);
	if (ret != SQLITE_OK)
	{
		dlog_print(DLOG_ERROR, SQLITE3_LOG_TAG,"Delete Error! [%s]", sqlite3_errmsg(sampleDb));
	   sqlite3_free(ErrMsg);
	   sqlite3_close(sampleDb);

	   return SQLITE_ERROR;
	}

	sqlite3_close(sampleDb);

   return SQLITE_OK;
}

int g_row_count = 0;

static int row_count_cb(void *data, int argc, char **argv, char **azColName)
{
	g_row_count = atoi(argv[0]); /*number of rows*/

	return 0;
}
int getTotalMsgItemsCount(int* num_of_rows)
{
	if(opendb() != SQLITE_OK) /*create database instance*/
		return SQLITE_ERROR;

   char *sql = "SELECT COUNT(*) FROM TizenSensorTable;";
   char *ErrMsg;

   int ret = 0;

   ret = sqlite3_exec(sampleDb, sql, row_count_cb, NULL, &ErrMsg);
	if (ret != SQLITE_OK)
	{
		dlog_print(DLOG_ERROR, SQLITE3_LOG_TAG,"Count Error! [%s]", sqlite3_errmsg(sampleDb));
	    sqlite3_free(ErrMsg);
	    sqlite3_close(sampleDb);

	    return SQLITE_ERROR;
	}

	dlog_print(DLOG_INFO, SQLITE3_LOG_TAG, "Total row found[%d]", g_row_count);

	sqlite3_close(sampleDb);

	*num_of_rows = g_row_count;
	g_row_count = 0;
   return SQLITE_OK;
}







static int internal_storage_id;
static bool storage_cb(int storage_id, storage_type_e type, storage_state_e state, const char *path, void *user_data)
{
    if (type == STORAGE_TYPE_INTERNAL) {
        internal_storage_id = storage_id;

        return false;
    }
    return true;
}


char* get_write_filepath(char *filename)
{

//	char *document_path;
//
////	document_path = app_get_data_path();
//
//	storage_foreach_device_supported(storage_cb, NULL);
//	storage_get_directory(internal_storage_id, STORAGE_DIRECTORY_DOCUMENTS, &document_path);
//
//	int size = strlen(document_path)+strlen(filename) + 1;
//	char * path = malloc(sizeof(char)*size);
//
//	strcpy(path, document_path);
//	strcat(path, "/");
//	strcat(path, filename);
//
//	return path;

	return "/opt/usr/home/owner/media/Documents/hda_sensor_data.txt";
}

char* write_file(char* filepath, char* buf)
{
    FILE *fp;
    fp = fopen(filepath,"w+");
    if(fp == NULL){
    	dlog_print(DLOG_ERROR, SQLITE3_LOG_TAG, "FILE CHECK: file is null about %s", filepath);
    	fclose(fp);
    	return "";
    }

    fputs(buf,fp);
    fclose(fp);
    return "";
}

char* append_file(char* filepath, char* buf)
{
    FILE *fp;
    fp = fopen(filepath,"a+");
    if(fp == NULL){
    	dlog_print(DLOG_ERROR, SQLITE3_LOG_TAG, "FILE CHECK: file is null about %s", filepath);
    	fclose(fp);
    	return "";
    }

    fputs(buf,fp);
    fclose(fp);
    return "";
}

char* read_file(char* filepath)
{
    FILE *fp;
    fp = fopen(filepath,"r");
    if(fp == NULL){
    	dlog_print(DLOG_ERROR, SQLITE3_LOG_TAG, "FILE CHECK: file is null about %s", filepath);
    	fclose(fp);
    	return "";
    }

    const int max = 512;
    char line[max];
    char * pLine;

    while(!feof(fp)){
    	pLine = fgets(line, max, fp);
    	dlog_print(DLOG_INFO, "FILE_SENSOR_VALUES", "%s", pLine);
    }
    fclose(fp);
    return "";
}
