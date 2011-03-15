#ifndef DB_H
#include <ruby.h>
#define DB_H
void db_main();
VALUE db_init(VALUE, VALUE, VALUE);
VALUE db_drop(VALUE);
VALUE db_command(VALUE, VALUE);
#endif
