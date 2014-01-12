/* Copyright (c) 2004, 2010, Oracle and/or its affiliates. All rights reserved.

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

#ifdef USE_PRAGMA_INTERFACE
#pragma interface			/* gcc class implementation */
#endif

#include "my_global.h"                   /* ulonglong */
#include "thr_lock.h"                    /* THR_LOCK, THR_LOCK_DATA */
#include "handler.h"                     /* handler */
#include "my_base.h"                     /* ha_rows */

class node{
public:

  uchar* data;
  node* next;
  int size;

  node(){
    data=NULL;
    next=NULL;
    size=0;
  }

  ~node(){
    free(data);
  }
};

node *first, *cur, *prev, *next;
int row_count, cur_pos;

void append_node(node* n);

typedef struct st_smalldb_share {
  char *table_name;
  uint table_name_length,use_count;
  mysql_mutex_t mutex;
  THR_LOCK lock;
} SMALLDB_SHARE;

class ha_smalldb: public handler
{
  THR_LOCK_DATA lock;
  SMALLDB_SHARE *share;

  String buffer;

public:
  ha_smalldb(handlerton *hton, TABLE_SHARE *table_arg);
  ~ha_smalldb()
  {
  }

  node* pack_row();
  void unpack_row(uchar* buf, node* n);

  const char *table_type() const { return "SMALLDB"; }

  const char *index_type(uint inx) { return "HASH"; }

  const char **bas_ext() const;

  ulonglong table_flags() const
  {
    return HA_BINLOG_STMT_CAPABLE;
  }

  ulong index_flags(uint inx, uint part, bool all_parts) const
  {
    return 0;
  }

  uint max_supported_record_length() const { return HA_MAX_REC_LENGTH; }

  uint max_supported_keys()          const { return 0; }

  uint max_supported_key_parts()     const { return 0; }

  uint max_supported_key_length()    const { return 0; }

  virtual double scan_time() { return (double) (stats.records+stats.deleted) / 20.0+10; }

  virtual double read_time(uint, uint, ha_rows rows)
  { return (double) rows /  20.0+1; }

  int open(const char *name, int mode, uint test_if_locked);    // required

  int close(void);                                              // required

  int write_row(uchar *buf);

  int update_row(const uchar *old_data, uchar *new_data);

  int delete_row(const uchar *buf);

  int index_read_map(uchar *buf, const uchar *key,
                     key_part_map keypart_map, enum ha_rkey_function find_flag);

  int index_next(uchar *buf);

  int index_prev(uchar *buf);

  int index_first(uchar *buf);

  int index_last(uchar *buf);

  int rnd_init(bool scan);                                      //required
  int rnd_end();
  int rnd_next(uchar *buf);                                     ///< required
  int rnd_pos(uchar *buf, uchar *pos);                          ///< required
  void position(const uchar *record);                           ///< required
  int info(uint);                                               ///< required
  int extra(enum ha_extra_function operation);
  int external_lock(THD *thd, int lock_type);                   ///< required
  int delete_all_rows(void);
  int truncate();
  ha_rows records_in_range(uint inx, key_range *min_key,
                           key_range *max_key);
  int delete_table(const char *from);
  int rename_table(const char * from, const char * to);
  int create(const char *name, TABLE *form,
             HA_CREATE_INFO *create_info);                      ///< required

  THR_LOCK_DATA **store_lock(THD *thd, THR_LOCK_DATA **to,
                             enum thr_lock_type lock_type);     ///< required
};
