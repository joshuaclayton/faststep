#include "bson_ruby_conversion.h"
#include "faststep_defines.h"

bson* create_bson_from_ruby_hash(const VALUE hash) {
  bson* document = (bson*)bson_malloc(sizeof(bson));

  if(NIL_P(hash)) {
    bson_empty(document);
  } else {
    VALUE byte_buffer = rb_funcall(rb_mBson, rb_intern("serialize"), 3, hash, Qfalse, Qfalse);
    VALUE query = rb_funcall(byte_buffer, rb_intern("to_s"), 0);
    bson* temp_bson = bson_malloc(sizeof(bson));
    bson_init(temp_bson, RSTRING_PTR(query), 0);
    bson_copy(document, temp_bson);
    bson_destroy(temp_bson);
  }

  return document;
}

bson* bson_from_ruby_array(const VALUE array) {
  VALUE hash = rb_hash_new();

  if(RTEST(array)) {
    int iterator;
    for(iterator = 0; iterator < RARRAY_LEN(array); iterator++) {
      rb_hash_aset(hash, rb_ary_entry(array, iterator), INT2NUM(1));
    }
  }

  return create_bson_from_ruby_hash(hash);
}

VALUE ruby_array_to_bson_ordered_hash(const VALUE array) {
  VALUE order_as_ordered_hash = rb_funcall(rb_cBsonOrderedHash, rb_intern("new"), 0);
  rb_iterate(rb_each, array, _map_assoc_ary_to_key_value_pair, order_as_ordered_hash);

  return order_as_ordered_hash;
}

VALUE ruby_hash_from_bson(const bson* bson) {
  VALUE bson_buf = rb_str_new(bson->data, bson_size(bson));

  return rb_funcall(rb_mBson, rb_intern("deserialize"), 1, bson_buf);
}

VALUE bool_to_ruby(const bson_bool_t result) {
  return result ? Qtrue : Qfalse;
}


VALUE ensure_document_ok(const VALUE document) {
  if(rb_funcall(rb_mFaststepSupport, rb_intern("ok?"), 1, document) == Qfalse) {
    rb_raise(rb_eFaststepOperationFailure, _invalid_command_description(document));
  }
  return Qtrue;
}

static char* _invalid_command_description(const VALUE document) {
  VALUE message = rb_str_new2("Invalid command (");
  rb_str_concat(message, rb_inspect(rb_hash_aref(document, rb_str_new2("bad cmd"))));
  rb_str_concat(message, rb_str_new2("): "));
  rb_str_concat(message, rb_hash_aref(document, rb_str_new2("errmsg")));

  return RSTRING_PTR(message);
}

static VALUE _map_assoc_ary_to_key_value_pair(const VALUE item, VALUE hash) {
  rb_hash_aset(hash, rb_ary_entry(item, 0), rb_ary_entry(item, 1));
  return hash;
}
