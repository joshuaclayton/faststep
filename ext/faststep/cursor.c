#include "cursor.h"
#include "bson_ruby_conversion.h"
#include "collection.h"
#include "faststep_defines.h"

void faststep_cursor_main() {
  rb_cFaststepCursor = rb_define_class_under(rb_mFaststep, "Cursor", rb_cObject);

  rb_define_attr(rb_cFaststepCursor, "collection", 1, 0);
  rb_include_module(rb_cFaststepCursor, rb_mEnumerable);

  rb_define_singleton_method(rb_cFaststepCursor, "new", faststep_cursor_new, -1);

  rb_define_method(rb_cFaststepCursor, "initialize", faststep_cursor_init, -1);
  rb_define_method(rb_cFaststepCursor, "explain",    faststep_cursor_explain, 0);
  rb_define_method(rb_cFaststepCursor, "skip",       faststep_cursor_skip, 1);
  rb_define_method(rb_cFaststepCursor, "limit",      faststep_cursor_limit, 1);
  rb_define_method(rb_cFaststepCursor, "fields",     faststep_cursor_fields, 1);
  rb_define_method(rb_cFaststepCursor, "order",      faststep_cursor_order, 1);
  rb_define_method(rb_cFaststepCursor, "each",       faststep_cursor_each, 0);

  return;
}

static VALUE faststep_cursor_init(int argc, VALUE* argv, VALUE self) {
  VALUE collection, options;
  rb_scan_args(argc, argv, "11", &collection, &options);

  rb_iv_set(self, "@collection", collection);

  VALUE selector = rb_hash_aref(options, rb_str_new2("selector"));
  if(NIL_P(selector)) { selector = rb_hash_new(); }
  if(RTEST(rb_hash_aref(selector, rb_str_new2("$query")))) {
    rb_iv_set(self, "@selector", rb_hash_aref(selector, rb_str_new2("$query")));
  } else {
    rb_iv_set(self, "@selector", selector);
  }

  rb_iv_set(self, "@explain", Qfalse);
  rb_iv_set(self, "@fields",  rb_hash_aref(options, rb_str_new2("fields")));
  rb_iv_set(self, "@skip",    rb_hash_aref(options, rb_str_new2("skip")));
  rb_iv_set(self, "@limit",   rb_hash_aref(options, rb_str_new2("limit")));

  return self;
}

static VALUE faststep_cursor_new(int argc, VALUE* argv, VALUE class) {
  faststep_cursor* fs_cursor = (faststep_cursor*)bson_malloc(sizeof(faststep_cursor));
  mongo_cursor* cursor = (mongo_cursor*)bson_malloc(sizeof(mongo_cursor));

  fs_cursor->cursor = cursor;
  fs_cursor->initialized = 0;

  VALUE tdata = Data_Wrap_Struct(class, NULL, _faststep_destroy_cursor, fs_cursor);

  rb_obj_call_init(tdata, argc, argv);
  return tdata;
}

static VALUE faststep_cursor_each(const VALUE self) {
  faststep_cursor* fs_cursor;
  Data_Get_Struct(self, faststep_cursor, fs_cursor);

  fs_cursor->cursor      = _faststep_build_mongo_cursor(self);
  fs_cursor->initialized = 1;

  while(mongo_cursor_next(fs_cursor->cursor)) {
    rb_yield(ruby_hash_from_bson(&fs_cursor->cursor->current));
  }
}

static VALUE faststep_cursor_explain(VALUE self) {
  rb_iv_set(self, "@explain", Qtrue);

  mongo_cursor* cursor = _faststep_build_mongo_cursor(self);
  mongo_cursor_next(cursor);

  VALUE result = ruby_hash_from_bson(&cursor->current);
  mongo_cursor_destroy(cursor);
  return result;
}

static VALUE faststep_cursor_skip(VALUE self, const VALUE skip_count) {
  rb_iv_set(self, "@skip", skip_count);
  return self;
}

static VALUE faststep_cursor_limit(VALUE self, const VALUE limit_count) {
  rb_iv_set(self, "@limit", limit_count);
  return self;
}

static VALUE faststep_cursor_fields(VALUE self, const VALUE fields) {
  rb_iv_set(self, "@fields", fields);
  return self;
}

static VALUE faststep_cursor_order(VALUE self, const VALUE order) {
  rb_iv_set(self, "@order", ruby_array_to_bson_ordered_hash(order));
  return self;
}

static mongo_cursor* _faststep_build_mongo_cursor(VALUE self) {
  bson* selector = create_bson_from_ruby_hash(_faststep_build_full_query(self));
  bson* fields   = bson_from_ruby_array(rb_iv_get(self, "@fields"));

  int limit = 0,
      skip  = 0;
  VALUE limit_value = rb_iv_get(self, "@limit"),
        skip_value  = rb_iv_get(self, "@skip");

  if(RTEST(limit_value)) { limit = -1*FIX2INT(limit_value); }
  if(RTEST(skip_value))  { skip  = FIX2INT(skip_value); }

  VALUE collection = rb_iv_get(self, "@collection");

  mongo_cursor* result = mongo_find(GetFaststepConnectionForCollection(collection),
                                    RSTRING_PTR(rb_funcall(collection, rb_intern("ns"), 0)),
                                    selector,
                                    fields,
                                    limit, skip, 0);

  bson_destroy(selector);
  bson_destroy(fields);

  return result;
}

static VALUE _faststep_build_full_query(VALUE self) {
  VALUE order_value    = rb_iv_get(self, "@order"),
        explain_value  = rb_iv_get(self, "@explain"),
        selector_value = rb_iv_get(self, "@selector");

  if(RTEST(explain_value) || RTEST(order_value)) {
    VALUE full_query = rb_hash_new();
    rb_hash_aset(full_query, rb_str_new2("$query"), selector_value);
    rb_hash_aset(full_query, rb_str_new2("$explain"), explain_value);
    if(RTEST(order_value)) {
      rb_hash_aset(full_query, rb_str_new2("$orderby"), order_value);
    }
    return full_query;
  } else {
    return selector_value;
  }
}

static void _faststep_destroy_cursor(faststep_cursor* cursor) {
  if(cursor->initialized == 1) {
    mongo_cursor_destroy(cursor->cursor);
    cursor->initialized = 0;
  }

  free(cursor);

  return;
}
