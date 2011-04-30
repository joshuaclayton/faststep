#ifndef DB_H
#include <ruby.h>
#define DB_H
void  faststep_db_main();
static VALUE faststep_db_drop(VALUE);
static VALUE faststep_db_command(VALUE, VALUE);
static VALUE faststep_db_get_last_error(VALUE);
#endif
