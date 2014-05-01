SqlGenerator
==============

Simple format in the input file for generating create table statements.  There are two types of records:

E <table_name>
R <parent_table_name> <child_table_name> <relation_type: [N|1]>
