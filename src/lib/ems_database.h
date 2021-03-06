/* Enna Media Server - a library and daemon for medias indexation and streaming
 *
 * Copyright (C) 2012 Enna Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _EMS_DATABASE_H_
#define _EMS_DATABASE_H_

#include "Ems.h"

int ems_database_init(void);
int ems_database_shutdown(void);
void ems_database_flush(void);

/* void ems_database_table_create(Ems_Database *db); */
void ems_database_file_insert(const char *hash, const char *place, const char *title,
                              int64_t mtime, int64_t start_time);
void ems_database_meta_insert(const char *hash, const char *meta, const char *value);
/* Eina_Value *ems_database_file_meta_get(Ems_Database *db, const char *filename, */
/*                                        const char *meta); */

Eina_List *ems_database_files_get(Ems_Db_Database *db);
/* const char *ems_database_file_get(Ems_Database *db, int item_id); */
const char *ems_database_file_uuid_get(char *filename);
int64_t ems_database_file_mtime_get(const char *hash);
void ems_database_deleted_files_remove(int64_t magic, const char *place);
/* Eina_List * ems_database_collection_get(Ems_Database *db, Ems_Collection *collection); */
/* const char *ems_database_uuid_get(Ems_Database *db); */
const char *ems_database_info_get(const char *sha1, const char *metadata);
const char *ems_database_uuid_get(void);

Ems_Db_Database *ems_database_get(const char *uuid);

#endif /* _EMS_DATABASE_H_ */
