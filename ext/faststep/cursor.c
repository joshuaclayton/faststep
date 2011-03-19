#include "cursor.h"
#include "bson_ruby_conversion.h"
#include "collection.h"
#include "faststep_defines.h"

void faststep_cursor_main() {
  rb_cFaststepCursor = rb_define_class_under(rb_mFaststep, "Cursor", rb_cObject);

  rb_define_attr(rb_cFaststepCursor, "collection", 1, 0);
  rb_include_module(rb_cFaststepCursor, rb_mEnumerable);

  rb_define_method(rb_cFaststepCursor, "initialize", faststep_cursor_init, 2);
  rb_define_method(rb_cFaststepCursor, "explain",    faststep_cursor_explain, 0);
  rb_define_method(rb_cFaststepCursor, "skip",       faststep_cursor_skip, 1);
  rb_define_method(rb_cFaststepCursor, "limit",      faststep_cursor_limit, 1);
  rb_define_method(rb_cFaststepCursor, "each",       faststep_cursor_each, 0);

  return;
}

static VALUE faststep_cursor_init(VALUE self, VALUE collection, VALUE options) {
  rb_iv_set(self, "@collection", collection);

  VALUE selector = rb_hash_aref(options, rb_str_new2("selector"));
  if(NIL_P(selector)) { selector = rb_hash_new(); }
  if(!NIL_P(rb_hash_aref(selector, rb_str_new2("$query")))) {
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

static VALUE faststep_cursor_each(VALUE self) {
  mongo_cursor* cursor = _faststep_build_mongo_cursor(self);

  while(mongo_cursor_next(cursor)) {
    rb_yield(ruby_hash_from_bson(&cursor->current));
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

static VALUE faststep_cursor_skip(VALUE self, VALUE skip_count) {
  rb_iv_set(self, "@skip", skip_count);
  return self;
}

static VALUE faststep_cursor_limit(VALUE self, VALUE limit_count) {
  rb_iv_set(self, "@limit", limit_count);
  return self;
}

static mongo_cursor* _faststep_build_mongo_cursor(VALUE self) {
  bson* selector = create_bson_from_ruby_hash(_faststep_build_full_query(self));
  bson* fields   = bson_from_ruby_array(rb_iv_get(self, "@fields"));

  int limit = 0;
  if(!NIL_P(rb_iv_get(self, "@limit"))) {
    limit = FIX2INT(rb_iv_get(self, "@limit"));
  }

  int skip = 0;
  if(!NIL_P(rb_iv_get(self, "@skip"))) {
    skip = FIX2INT(rb_iv_get(self, "@skip"));
  }

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
  if(rb_iv_get(self, "@explain") == Qtrue) {
    VALUE full_query = rb_hash_new();
    rb_hash_aset(full_query, rb_str_new2("$query"),   rb_iv_get(self, "@selector"));
    rb_hash_aset(full_query, rb_str_new2("$explain"), rb_iv_get(self, "@explain"));
    return full_query;
  } else {
    return rb_iv_get(self, "@selector");
  }
}
