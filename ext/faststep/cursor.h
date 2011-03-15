#ifndef CURSOR_H
#include <ruby.h>
#include "mongo.h"
#define CURSOR_h
void cursor_main();
VALUE cursor_init(VALUE, VALUE, VALUE);
VALUE cursor_new(VALUE, VALUE, VALUE);
VALUE cursor_each(VALUE);
mongo_cursor* build_mongo_cursor(VALUE, VALUE);
#endif
