#include "bson_ruby_conversion.h"
#include "faststep_defines.h"

bson* create_bson_from_ruby_hash(const VALUE hash) {
  bson* document = (bson*)bson_malloc(sizeof(bson));

  if(NIL_P(hash)) {
    static char *empty = "\005\0\0\0\0";
    char *data = bson_malloc(sizeof(empty));
    strncpy(data, empty, sizeof(empty));

    bson_init_data(document, data);
  } else {
    VALUE byte_buffer = rb_funcall(rb_mBson, rb_intern("serialize"), 3, hash, Qfalse, Qfalse);
    VALUE serialized_bson = rb_funcall(byte_buffer, rb_intern("to_s"), 0);

    char* data = bson_malloc(RSTRING_LEN(serialized_bson));
    memmove(data, RSTRING_PTR(serialized_bson), RSTRING_LEN(serialized_bson));

    bson_init_data(document, data);
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

void faststep_bson_destroy(bson* document) {
  bson_destroy(document);
  free(document);
}

VALUE ensure_document_ok(const VALUE document) {
  if(rb_funcall(rb_mFaststepSupport, rb_intern("ok?"), 1, document) == Qfalse) {
    VALUE e = rb_exc_new3(rb_eFaststepOperationFailure, _invalid_command_description(document));
    rb_exc_raise(e);
  }
  return Qtrue;
}

static VALUE _invalid_command_description(const VALUE document) {
  VALUE message = rb_str_new2("");

  if(RTEST(rb_hash_aref(document, rb_str_new2("bad cmd")))) {
    rb_str_concat(message, rb_str_new2("Invalid command ("));
    rb_str_concat(message, rb_inspect(rb_hash_aref(document, rb_str_new2("bad cmd"))));
    rb_str_concat(message, rb_str_new2("): "));
    rb_str_concat(message, rb_hash_aref(document, rb_str_new2("errmsg")));
  } else {
    rb_str_concat(message, rb_str_new2("Error ("));
    rb_str_concat(message, rb_inspect(rb_hash_aref(document, rb_str_new2("code"))));
    rb_str_concat(message, rb_str_new2("): "));
    rb_str_concat(message, rb_hash_aref(document, rb_str_new2("err")));
  }

  return message;
}

static VALUE _map_assoc_ary_to_key_value_pair(const VALUE item, VALUE hash) {
  rb_funcall(hash, rb_intern("[]="), 2, rb_ary_entry(item, 0), rb_ary_entry(item, 1));
  return hash;
}
