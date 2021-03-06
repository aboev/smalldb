## SmallDB

SmallDB - sample in-memory storage engine for MySQL  
Supports basic CRUD operations, data is stored in a linked list

## Installation

Installation requires MySQL source code(same as installed version)  
Following example is for MySQL Community 5.5.35  

1. Download & untar MySQL source code  

```
  wget http://downloads.mysql.com/archives/get/file/mysql-5.5.35.tar.gz
  tar -zxvf mysql-5.5.35.tar.gz
```
2. Clone smalldb into mysql-5.5.35/storage/smalldb directory

```
  cd mysql-5.5.35
  git clone https://github.com/aboev/smalldb.git storage/smalldb
```
3. Run cmake

```
  cmake .
```
4. (Optional) Install & run MySQL if you don't have one. Please follow instructions on http://dev.mysql.com/doc/refman/5.6/en/installing-source-distribution.html for post-install setup  

```
  make  
  make install
```
5. Compile smalldb 

```
  cd storage/smalldb  
  make
```
5. Copy smalldb.so file into MySQL plugin directory (You can check plugin directory through: SHOW VARIABLES LIKE "%plugin_dir%";)  

```
  cp ha_smalldb.so /opt/mysql/server-5.5.35/lib/plugin/
```
6. Install smalldb

```
  mysql -u root -p -e 'INSTALL PLUGIN smalldb soname "ha_smalldb.so";'
```

Now you can define smalldb as storage engine:  
```
CREATE TABLE tbl (col VARCHAR(255)) ENGINE = smalldb;
```
