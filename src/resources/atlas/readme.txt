Atlas Howto
-----------

Prereqisites
------------
- Install SQLite3 command line interface on your machine
- Install Perl and SQLite 3 driver (something like libdbd-sqlite3)
- Download and unpack required files from geonames.org download center and move them into the folder "data"

Required files
--------------
Download from http://download.geonames.org/export/dump/

timeZones.txt
countryInfo.txt
featureCodes_en.txt
admin1CodesASCII.txt

and one of
- cities15000.txt (cities with more than 15000 inhabitants, about 3.5MB)
- cities1000.txt (more than 1000 inhabitants, about 16MB)
- cities500.txt (more than 500 inhabitants, about 82MB)
- allCountries.txt (all, about 940MB)


Create tables
-------------
Start SQLite command line tool, import db_ddl.sql 

Example (Linux)

~/.config/saravali9/atlas> sqlite3 atlas.db
SQLite version 3.6.22
Enter ".help" for instructions
Enter SQL statements terminated with a ";"
sqlite> .read db_ddl.sql 
sqlite> 
sqlite> 
sqlite> .quit

Import data
-----------
Edit Python script import_geoname.py if necessary
- name of city file
- name of database

Start script

Export data to sql file
-----------------------
Create the sql file (e.g. geonames1000.sql) contained in the release. DB must be created and populated first, see above.

~/.config/saravali9/atlas> sqlite3 atlas.db
SQLite version 3.53.3 2026-06-26 20:14:12
Enter ".help" for usage hints.
sqlite> .output geonames1000.sql
sqlite> .dump
sqlite> .quit




