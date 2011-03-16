#include "cursor.h"
#include "bson_ruby_conversion.h"
#include "collection.h"
#include "faststep_defines.h"

void faststep_cursor_main() {
  rb_cFaststepCursor = rb_define_class_under(rb_mFaststep, "Cursor", rb_cObject);

  rb_define_attr(rb_cFaststepCursor, "collection", 1, 0);
  rb_include_module(rb_cFaststepCursor, rb_mEnumerable);

  rb_define_singleton_method(rb_cFaststepCursor, "new", faststep_cursor_new, 2);

  rb_define_method(rb_cFaststepCursor, "initialize", faststep_cursor_init, 2);
  rb_define_method(rb_cFaststepCursor, "each", faststep_cursor_each, 0);

  return;
}

static VALUE faststep_cursor_init(VALUE self, VALUE collection, VALUE options) {
  rb_iv_set(self, "@collection", collection);

  return self;
}

static VALUE faststep_cursor_new(VALUE class, VALUE collection, VALUE options) {
  mongo_cursor* cursor = _faststep_build_mongo_cursor(collection, options);

  VALUE tdata = Data_Wrap_Struct(class, NULL, mongo_cursor_destroy, cursor);

  VALUE argv[2];
  argv[0] = collection;
  argv[1] = options;

  rb_obj_call_init(tdata, 2, argv);

  return tdata;
}

static VALUE faststep_cursor_each(VALUE self) {
  mongo_cursor* cursor;
  Data_Get_Struct(self, mongo_cursor, cursor);

  while(mongo_cursor_next(cursor)) {
    rb_yield(ruby_hash_from_bson(&cursor->current));
  }
}

static mongo_cursor* _faststep_build_mongo_cursor(VALUE collection, VALUE options) {
  bson* selector = create_bson_from_ruby_hash(rb_hash_aref(options, rb_str_new2("selector")));

  mongo_cursor* result = mongo_find(GetFaststepConnectionForCollection(collection),
                                    RSTRING_PTR(rb_funcall(collection, rb_intern("ns"), 0)),
                                    selector,
                                    NULL,
                                    0, 0, 0);

  bson_destroy(selector);

  return result;
}
