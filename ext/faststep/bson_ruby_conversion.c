#include "bson_ruby_conversion.h"

void init_bson_from_ruby_hash(bson* bson, VALUE hash) {
  if(NIL_P(hash)) {
    bson_empty(bson);
  } else {
    VALUE rb_bson = rb_const_get(rb_cObject, rb_intern("BSON"));
    VALUE byte_buffer = rb_funcall(rb_bson, rb_intern("serialize"), 3, hash, Qfalse, Qfalse);
    VALUE query = rb_funcall(byte_buffer, rb_intern("to_s"), 0);
    bson_init(bson, RSTRING_PTR(query), 1);
  }
  return;
}

VALUE ruby_hash_from_bson(bson* bson) {
  VALUE rb_bson = rb_const_get(rb_cObject, rb_intern("BSON"));
  VALUE bson_buf = rb_str_new(bson->data, bson_size(bson));

  return rb_funcall(rb_bson, rb_intern("deserialize"), 1, bson_buf);
}
