#include "cursor.h"
#include "bson_ruby_conversion.h"
#include "collection.h"

void cursor_main(VALUE faststep) {
  VALUE FaststepCursor = rb_define_class_under(faststep, "Cursor", rb_cObject);

  rb_define_attr(FaststepCursor, "collection", 1, 0);
  rb_include_module(FaststepCursor, rb_mEnumerable);

  rb_define_singleton_method(FaststepCursor, "new", cursor_new, 2);

  rb_define_method(FaststepCursor, "initialize", cursor_init, 2);
  rb_define_method(FaststepCursor, "each", cursor_each, 0);

  return;
}

VALUE cursor_init(VALUE self, VALUE collection, VALUE options) {
  rb_iv_set(self, "@collection", collection);

  return self;
}

mongo_cursor* build_mongo_cursor(VALUE collection, VALUE options) {
  VALUE db = rb_iv_get(collection, "@db");
  VALUE faststep_conn = rb_iv_get(db, "@connection");

  mongo_connection* conn;
  Data_Get_Struct(faststep_conn, mongo_connection, conn);

  bson* selector = bson_malloc(sizeof(bson));

  init_bson_from_ruby_hash(selector, rb_hash_aref(options, rb_str_new2("selector")));

  char* ns = RSTRING_PTR(rb_funcall(collection, rb_intern("ns"), 0));

  return mongo_find(conn, ns, selector, NULL, 0, 0, 0);
}

VALUE cursor_new(VALUE class, VALUE collection, VALUE options) {
  mongo_cursor* cursor = build_mongo_cursor(collection, options);

  VALUE tdata = Data_Wrap_Struct(class, NULL, mongo_cursor_destroy, cursor);

  VALUE argv[2];
  argv[0] = collection;
  argv[1] = options;

  rb_obj_call_init(tdata, 2, argv);

  return tdata;
}

VALUE cursor_each(VALUE self) {
  mongo_cursor* cursor;
  Data_Get_Struct(self, mongo_cursor, cursor);

  while(mongo_cursor_next(cursor)) {
    rb_yield(ruby_hash_from_bson(&cursor->current));
  }
}
