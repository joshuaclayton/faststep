#ifndef CURSOR_H
#include <ruby.h>
#include "mongo.h"
#define CURSOR_h

typedef struct {
  mongo_cursor* cursor;
  int initialized;
} faststep_cursor;

void  faststep_cursor_main();
static VALUE faststep_cursor_init(int, VALUE*, VALUE);
static VALUE faststep_cursor_new(int, VALUE*, VALUE);
static VALUE faststep_cursor_each(const VALUE);
static VALUE faststep_cursor_explain(VALUE);
static VALUE faststep_cursor_order(VALUE, const VALUE);
static mongo_cursor* _faststep_build_mongo_cursor(VALUE);
static VALUE _faststep_build_full_query(VALUE);
static void  _faststep_destroy_cursor(faststep_cursor*);

#endif
