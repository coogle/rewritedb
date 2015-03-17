# DB Schema #
This is the current database schema of the RewriteDB database for your reference

```
create table rewrites(id integer auto_increment, rewriteURL varchar(1024), groupID integer);
create table groups(id integer auto_increment, group_name varchar(1024));
create table group_members(group_id integer, url varchar(4096));
create table global_settings(setting varchar(50), value);
```