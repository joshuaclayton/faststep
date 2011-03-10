#ifndef DB_H
#include <ruby.h>
#define DB_H
void db_main(VALUE);
VALUE db_init(VALUE, VALUE, VALUE);
VALUE db_drop(VALUE);
#endif
