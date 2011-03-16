#ifndef CURSOR_H
#include <ruby.h>
#include "mongo.h"
#define CURSOR_h
void  faststep_cursor_main();
VALUE faststep_cursor_init(VALUE, VALUE, VALUE);
VALUE faststep_cursor_new(VALUE, VALUE, VALUE);
VALUE faststep_cursor_each(VALUE);
mongo_cursor* faststep_build_mongo_cursor(VALUE, VALUE);
#endif
