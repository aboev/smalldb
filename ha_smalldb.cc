/* Copyright (c) 2004, 2011, Oracle and/or its affiliates. All rights reserved.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; version 2 of the License.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA */

#ifdef USE_PRAGMA_IMPLEMENTATION
#pragma implementation        // gcc: Class implementation
#endif

#include "sql_priv.h"
#include "sql_class.h"           // MYSQL_HANDLERTON_INTERFACE_VERSION
#include "ha_smalldb.h"
#include "probes_mysql.h"
#include "sql_plugin.h"

static handler *smalldb_create_handler(handlerton *hton,
                                       TABLE_SHARE *table, 
                                       MEM_ROOT *mem_root);

handlerton *smalldb_hton;

/* Interface to mysqld, to check system tables supported by SE */
static const char* smalldb_system_database();
static bool smalldb_is_supported_system_table(const char *db,
                                      const char *table_name,
                                      bool is_sql_layer_system_table);

static HASH smalldb_open_tables;

mysql_mutex_t smalldb_mutex;

static uchar* smalldb_get_key(SMALLDB_SHARE *share, size_t *length,
                             my_bool not_used __attribute__((unused)))
{
  *length=share->table_name_length;
  return (uchar*) share->table_name;
}

#ifdef HAVE_PSI_INTERFACE
static PSI_mutex_key ex_key_mutex_smalldb, ex_key_mutex_SMALLDB_SHARE_mutex;

static PSI_mutex_info all_smalldb_mutexes[]=
{
  { &ex_key_mutex_smalldb, "smalldb", PSI_FLAG_GLOBAL},
  { &ex_key_mutex_SMALLDB_SHARE_mutex, "SMALLDB_SHARE::mutex", 0}
};

static void init_smalldb_psi_keys()
{
  const char* category= "smalldb";
  int count;

  if (PSI_server == NULL)
    return;

  count= array_elements(all_smalldb_mutexes);
  PSI_server->register_mutex(category, all_smalldb_mutexes, count);
}
#endif


static int smalldb_init_func(void *p)
{
  DBUG_ENTER("smalldb_init_func");

#ifdef HAVE_PSI_INTERFACE
  init_smalldb_psi_keys();
#endif

  smalldb_hton= (handlerton *)p;
  mysql_mutex_init(ex_key_mutex_smalldb, &smalldb_mutex, MY_MUTEX_INIT_FAST);
  (void) my_hash_init(&smalldb_open_tables,system_charset_info,32,0,0,
                      (my_hash_get_key) smalldb_get_key,0,0);

  smalldb_hton->state=   SHOW_OPTION_YES;
  smalldb_hton->create=  smalldb_create_handler;
  smalldb_hton->flags=   HTON_CAN_RECREATE;
  smalldb_hton->system_database=   smalldb_system_database;
  smalldb_hton->is_supported_system_table= smalldb_is_supported_system_table;

  DBUG_RETURN(0);
}


static int smalldb_done_func(void *p)
{
  int error= 0;
  DBUG_ENTER("smalldb_done_func");

  if (smalldb_open_tables.records)
    error= 1;
  my_hash_free(&smalldb_open_tables);
  mysql_mutex_destroy(&smalldb_mutex);

  DBUG_RETURN(error);
}


static SMALLDB_SHARE *get_share(const char *table_name, TABLE *table)
{
  SMALLDB_SHARE *share;
  uint length;
  char *tmp_name;

  mysql_mutex_lock(&smalldb_mutex);
  length=(uint) strlen(table_name);

  if (!(share=(SMALLDB_SHARE*) my_hash_search(&smalldb_open_tables,
                                              (uchar*) table_name,
                                              length)))
  {
    if (!(share=(SMALLDB_SHARE *)
          my_multi_malloc(MYF(MY_WME | MY_ZEROFILL),
                          &share, sizeof(*share),
                          &tmp_name, length+1,
                          NullS)))
    {
      mysql_mutex_unlock(&smalldb_mutex);
      return NULL;
    }

    share->use_count=0;
    share->table_name_length=length;
    share->table_name=tmp_name;
    strmov(share->table_name,table_name);
    if (my_hash_insert(&smalldb_open_tables, (uchar*) share))
      goto error;
    thr_lock_init(&share->lock);
    mysql_mutex_init(ex_key_mutex_SMALLDB_SHARE_mutex,
                     &share->mutex, MY_MUTEX_INIT_FAST);
  }
  share->use_count++;
  mysql_mutex_unlock(&smalldb_mutex);

  return share;

error:
  mysql_mutex_destroy(&share->mutex);
  my_free(share);

  return NULL;
}


static int free_share(SMALLDB_SHARE *share)
{
  mysql_mutex_lock(&smalldb_mutex);
  if (!--share->use_count)
  {
    my_hash_delete(&smalldb_open_tables, (uchar*) share);
    thr_lock_delete(&share->lock);
    mysql_mutex_destroy(&share->mutex);
    my_free(share);
  }
  mysql_mutex_unlock(&smalldb_mutex);

  return 0;
}

static handler* smalldb_create_handler(handlerton *hton,
                                       TABLE_SHARE *table, 
                                       MEM_ROOT *mem_root)
{
  return new (mem_root) ha_smalldb(hton, table);
}

ha_smalldb::ha_smalldb(handlerton *hton, TABLE_SHARE *table_arg)
  :handler(hton, table_arg)
{
  first=cur=prev=next=NULL;
  row_count=cur_pos=0;
}


static const char *ha_smalldb_exts[] = {
  NullS
};

const char **ha_smalldb::bas_ext() const
{
  return ha_smalldb_exts;
}

const char* ha_smalldb_system_database= NULL;
const char* smalldb_system_database()
{
  return ha_smalldb_system_database;
}

static st_system_tablename ha_smalldb_system_tables[]= {
  {(const char*)NULL, (const char*)NULL}
};

static bool smalldb_is_supported_system_table(const char *db,
                                              const char *table_name,
                                              bool is_sql_layer_system_table)
{
  st_system_tablename *systab;

  // Does this SE support "ALL" SQL layer system tables ?
  if (is_sql_layer_system_table)
    return false;

  // Check if this is SE layer system tables
  systab= ha_smalldb_system_tables;
  while (systab && systab->db)
  {
    if (systab->db == db &&
        strcmp(systab->tablename, table_name) == 0)
      return true;
    systab++;
  }

  return false;
}


int ha_smalldb::open(const char *name, int mode, uint test_if_locked)
{
  DBUG_ENTER("ha_smalldb::open");

  if (!(share = get_share(name, table)))
    DBUG_RETURN(1);
  thr_lock_data_init(&share->lock,&lock,NULL);

  DBUG_RETURN(0);
}

int ha_smalldb::close(void)
{
  DBUG_ENTER("ha_smalldb::close");
  DBUG_RETURN(free_share(share));
}


int ha_smalldb::write_row(uchar *buf)
{
  DBUG_ENTER("ha_smalldb::write_row");

  node* n=new node();
  n->data = (uchar*) malloc(sizeof(uchar)*table->s->reclength);
  memcpy(n->data,buf,table->s->reclength);

  append_node(n);
  row_count++;

  DBUG_RETURN(0);
}


int ha_smalldb::update_row(const uchar *old_data, uchar *new_data)
{

  DBUG_ENTER("ha_smalldb::update_row");
  DBUG_RETURN(HA_ERR_WRONG_COMMAND);
}


int ha_smalldb::delete_row(const uchar *buf)
{
  DBUG_ENTER("ha_smalldb::delete_row");

  if (cur!=first){
    free(cur);
    prev->next=next;
  }else{
    free(cur);
    cur=NULL;
    first=next;
  }
  row_count--;

  DBUG_RETURN(0);
}


int ha_smalldb::index_read_map(uchar *buf, const uchar *key,
                               key_part_map keypart_map __attribute__((unused)),
                               enum ha_rkey_function find_flag
                               __attribute__((unused)))
{
  int rc;
  DBUG_ENTER("ha_smalldb::index_read");
  MYSQL_INDEX_READ_ROW_START(table_share->db.str, table_share->table_name.str);
  rc= HA_ERR_WRONG_COMMAND;
  MYSQL_INDEX_READ_ROW_DONE(rc);
  DBUG_RETURN(rc);
}


int ha_smalldb::index_next(uchar *buf)
{
  int rc;
  DBUG_ENTER("ha_smalldb::index_next");
  MYSQL_INDEX_READ_ROW_START(table_share->db.str, table_share->table_name.str);
  rc= HA_ERR_WRONG_COMMAND;
  MYSQL_INDEX_READ_ROW_DONE(rc);
  DBUG_RETURN(rc);
}


int ha_smalldb::index_prev(uchar *buf)
{
  int rc;
  DBUG_ENTER("ha_smalldb::index_prev");
  MYSQL_INDEX_READ_ROW_START(table_share->db.str, table_share->table_name.str);
  rc= HA_ERR_WRONG_COMMAND;
  MYSQL_INDEX_READ_ROW_DONE(rc);
  DBUG_RETURN(rc);
}


int ha_smalldb::index_first(uchar *buf)
{
  int rc;
  DBUG_ENTER("ha_smalldb::index_first");
  MYSQL_INDEX_READ_ROW_START(table_share->db.str, table_share->table_name.str);
  rc= HA_ERR_WRONG_COMMAND;
  MYSQL_INDEX_READ_ROW_DONE(rc);
  DBUG_RETURN(rc);
}


int ha_smalldb::index_last(uchar *buf)
{
  int rc;
  DBUG_ENTER("ha_smalldb::index_last");
  MYSQL_INDEX_READ_ROW_START(table_share->db.str, table_share->table_name.str);
  rc= HA_ERR_WRONG_COMMAND;
  MYSQL_INDEX_READ_ROW_DONE(rc);
  DBUG_RETURN(rc);
}


int ha_smalldb::rnd_init(bool scan)
{
  DBUG_ENTER("ha_smalldb::rnd_init");

  cur=NULL;
  prev=NULL;
  next=first;
  cur_pos=0;

  DBUG_RETURN(0);
}

int ha_smalldb::rnd_end()
{
  DBUG_ENTER("ha_smalldb::rnd_end");
  DBUG_RETURN(0);
}

int ha_smalldb::rnd_next(uchar *buf)
{
  int rc;
  DBUG_ENTER("ha_smalldb::rnd_next");
  MYSQL_READ_ROW_START(table_share->db.str, table_share->table_name.str,
                       TRUE);

  if (next!=NULL){
    prev=cur;
    cur=next;
    next=cur->next;
    memcpy(buf,cur->data,table->s->reclength);
    cur_pos++;
    rc=0;
  }else{
    rc= HA_ERR_END_OF_FILE;
  }

  MYSQL_READ_ROW_DONE(rc);
  DBUG_RETURN(rc);
}


void ha_smalldb::position(const uchar *record)
{
  DBUG_ENTER("ha_smalldb::position");
  DBUG_VOID_RETURN;
}


int ha_smalldb::rnd_pos(uchar *buf, uchar *pos)
{
  int rc;
  DBUG_ENTER("ha_smalldb::rnd_pos");
  MYSQL_READ_ROW_START(table_share->db.str, table_share->table_name.str,
                       TRUE);
  rc= HA_ERR_WRONG_COMMAND;
  MYSQL_READ_ROW_DONE(rc);
  DBUG_RETURN(rc);
}


int ha_smalldb::info(uint flag)
{
  DBUG_ENTER("ha_smalldb::info");
  DBUG_RETURN(0);
}


int ha_smalldb::extra(enum ha_extra_function operation)
{
  DBUG_ENTER("ha_smalldb::extra");
  DBUG_RETURN(0);
}


int ha_smalldb::delete_all_rows()
{
  DBUG_ENTER("ha_smalldb::delete_all_rows");
  DBUG_RETURN(HA_ERR_WRONG_COMMAND);
}


int ha_smalldb::truncate()
{
  DBUG_ENTER("ha_smalldb::truncate");
  DBUG_RETURN(HA_ERR_WRONG_COMMAND);
}


int ha_smalldb::external_lock(THD *thd, int lock_type)
{
  DBUG_ENTER("ha_smalldb::external_lock");
  DBUG_RETURN(0);
}


THR_LOCK_DATA **ha_smalldb::store_lock(THD *thd,
                                       THR_LOCK_DATA **to,
                                       enum thr_lock_type lock_type)
{
  if (lock_type != TL_IGNORE && lock.type == TL_UNLOCK)
    lock.type=lock_type;
  *to++= &lock;
  return to;
}


int ha_smalldb::delete_table(const char *name)
{
  DBUG_ENTER("ha_smalldb::delete_table");
  /* This is not implemented but we want someone to be able that it works. */
  DBUG_RETURN(0);
}


int ha_smalldb::rename_table(const char * from, const char * to)
{
  DBUG_ENTER("ha_smalldb::rename_table ");
  DBUG_RETURN(HA_ERR_WRONG_COMMAND);
}


ha_rows ha_smalldb::records_in_range(uint inx, key_range *min_key,
                                     key_range *max_key)
{
  DBUG_ENTER("ha_smalldb::records_in_range");
  DBUG_RETURN(10);                         // low number to force index usage
}


int ha_smalldb::create(const char *name, TABLE *table_arg,
                       HA_CREATE_INFO *create_info)
{
  DBUG_ENTER("ha_smalldb::create");
  /*
    This is not implemented but we want someone to be able to see that it
    works.
  */
  DBUG_RETURN(0);
}

void append_node(node* n){
  if (first==NULL){
    first=n;
  }else{
    node* iter=first;
    while (iter->next!=NULL){iter=iter->next;}
    iter->next=n;
  }
}

struct st_mysql_storage_engine smalldb_storage_engine=
{ MYSQL_HANDLERTON_INTERFACE_VERSION };

static ulong srv_enum_var= 0;
static ulong srv_ulong_var= 0;
static double srv_double_var= 0;

const char *enum_var_names[]=
{
  "e1", "e2", NullS
};

TYPELIB enum_var_typelib=
{
  array_elements(enum_var_names) - 1, "enum_var_typelib",
  enum_var_names, NULL
};

static MYSQL_SYSVAR_ENUM(
  enum_var,                       // name
  srv_enum_var,                   // varname
  PLUGIN_VAR_RQCMDARG,            // opt
  "Sample ENUM system variable.", // comment
  NULL,                           // check
  NULL,                           // update
  0,                              // def
  &enum_var_typelib);             // typelib

static MYSQL_SYSVAR_ULONG(
  ulong_var,
  srv_ulong_var,
  PLUGIN_VAR_RQCMDARG,
  "0..1000",
  NULL,
  NULL,
  8,
  0,
  1000,
  0);

static MYSQL_SYSVAR_DOUBLE(
  double_var,
  srv_double_var,
  PLUGIN_VAR_RQCMDARG,
  "0.500000..1000.500000",
  NULL,
  NULL,
  8.5,
  0.5,
  1000.5,
  0);                             // reserved always 0

static MYSQL_THDVAR_DOUBLE(
  double_thdvar,
  PLUGIN_VAR_RQCMDARG,
  "0.500000..1000.500000",
  NULL,
  NULL,
  8.5,
  0.5,
  1000.5,
  0);

static struct st_mysql_sys_var* smalldb_system_variables[]= {
  MYSQL_SYSVAR(enum_var),
  MYSQL_SYSVAR(ulong_var),
  MYSQL_SYSVAR(double_var),
  MYSQL_SYSVAR(double_thdvar),
  NULL
};

static int show_func_smalldb(MYSQL_THD thd, struct st_mysql_show_var *var,
                             char *buf)
{
  var->type= SHOW_CHAR;
  var->value= buf; // it's of SHOW_VAR_FUNC_BUFF_SIZE bytes
  my_snprintf(buf, SHOW_VAR_FUNC_BUFF_SIZE,
              "enum_var is %lu, ulong_var is %lu, "
              "double_var is %f, %.6b", // %b is a MySQL extension
              srv_enum_var, srv_ulong_var, srv_double_var, "really");
  return 0;
}

static struct st_mysql_show_var func_status[]=
{
  {"smalldb_func_smalldb",  (char *)show_func_smalldb, SHOW_FUNC},
  {0,0,SHOW_UNDEF}
};

mysql_declare_plugin(smalldb)
{
  MYSQL_STORAGE_ENGINE_PLUGIN,
  &smalldb_storage_engine,
  "SMALLDB",
  "Aleksey B",
  "smalldb storage engine",
  PLUGIN_LICENSE_GPL,
  smalldb_init_func,                            /* Plugin Init */
  smalldb_done_func,                            /* Plugin Deinit */
  0x0001 /* 0.1 */,
  func_status,                                  /* status variables */
  smalldb_system_variables,                     /* system variables */
  NULL,                                         /* config options */
  0,                                            /* flags */
}
mysql_declare_plugin_end;

