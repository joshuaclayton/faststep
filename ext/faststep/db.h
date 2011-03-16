#ifndef DB_H
#include <ruby.h>
#define DB_H
void  faststep_db_main();
VALUE faststep_db_init(VALUE, VALUE, VALUE);
VALUE faststep_db_drop(VALUE);
VALUE faststep_db_command(VALUE, VALUE);
#endif
