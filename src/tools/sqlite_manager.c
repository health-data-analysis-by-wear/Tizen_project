#include <sqlite3.h>

#define BUFLEN 300 /* Buffer size, used in functions */
const int key_len = 256; /* AES key length */

/* Password to generate key */
static const unsigned char password[] = {"DummyPassword"};

sqlite3 *db; /* Database handle */

unsigned char salt[9]; /* Encryption salt */
unsigned char iv[17]; /* Encryption initial vector */

 void sqlite3_manager_initialize(){
	 sqlite3_shutdown();
	 slqite3_config(SQLITE_CONFIG_URI, 1);

	 sqlite3_initialize();
 }

 void sqlite3_manager_open(char *db_name){
	 char file_path[BUFLEN];
	 char *document_path;
	 /*
	    Fill the variable with the value obtained
	    using storage_foreach_device_supported()
	 */
	 int internal_storage_id = 0;
	 storage_get_directory(internal_storage_id, STORAGE_DIRECTORY_DOCUMENTS, &document_path);
	 snprintf(file_path, size, "%s/%s", document_path, db_name);
	 free(document_path);

	 sqlite3_open(file_path, &db);

	 free(file_path);
 }
