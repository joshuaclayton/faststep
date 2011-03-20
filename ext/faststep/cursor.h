#ifndef CURSOR_H
#include <ruby.h>
#include "mongo.h"
#define CURSOR_h
void  faststep_cursor_main();
static VALUE faststep_cursor_init(int, VALUE*, VALUE);
static VALUE faststep_cursor_each(const VALUE);
static VALUE faststep_cursor_explain(VALUE);
static VALUE faststep_cursor_skip(VALUE, const VALUE);
static VALUE faststep_cursor_limit(VALUE, const VALUE);
static VALUE faststep_cursor_fields(VALUE, const VALUE);
static VALUE faststep_cursor_order(VALUE, const VALUE);
static mongo_cursor* _faststep_build_mongo_cursor(VALUE);
static VALUE _faststep_build_full_query(VALUE);
#endif
