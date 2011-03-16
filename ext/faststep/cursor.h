#ifndef CURSOR_H
#include <ruby.h>
#include "mongo.h"
#define CURSOR_h
void  faststep_cursor_main();
static VALUE faststep_cursor_init(VALUE, VALUE, VALUE);
static VALUE faststep_cursor_new(VALUE, VALUE, VALUE);
static VALUE faststep_cursor_each(VALUE);
static mongo_cursor* _faststep_build_mongo_cursor(VALUE, VALUE);
#endif
