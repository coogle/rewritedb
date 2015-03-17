# Introduction #

RewriteDB is a C application which is designed to be used with the Apache Webserver [mod\_rewrite](http://httpd.apache.org/docs/1.3/mod/mod_rewrite.html)module. It allows you to dynamically map arbitrary mod\_rewrite URLs to one or more real URLs. It is useful for site management, or to increase search engine visibility. The backend datbaase is [SQLite](http://www.sqlite.org), meaning you can also manipulate the rewrite rules using a language such as [PHP](http://www.php.net) without having to reboot the web server.

# Useful Links #
  * [SQLite Homepage](http://www.sqlite.org)
  * [RewriteDBSchema](RewriteDBSchema.md) - The Rewrite DB SQLite Schema