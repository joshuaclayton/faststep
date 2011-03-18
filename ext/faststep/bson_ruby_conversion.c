#include "bson_ruby_conversion.h"
#include "faststep_defines.h"

bson* create_bson_from_ruby_hash(VALUE hash) {
  bson* document = bson_malloc(sizeof(bson));

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

VALUE ruby_hash_from_bson(bson* bson) {
  VALUE bson_buf = rb_str_new(bson->data, bson_size(bson));

  return rb_funcall(rb_mBson, rb_intern("deserialize"), 1, bson_buf);
}

VALUE ensure_document_ok(VALUE document) {
  if(rb_funcall(rb_mFaststepSupport, rb_intern("ok?"), 1, document) == Qfalse) {
    rb_raise(rb_cFaststepOperationFailure, _invalid_command_description(document));
  }
}

static char* _invalid_command_description(VALUE document) {
  VALUE message = rb_str_new2("Invalid command (");
  rb_str_concat(message, rb_funcall(rb_hash_aref(document, rb_str_new2("bad cmd")), rb_intern("inspect"), 0));
  rb_str_concat(message, rb_str_new2("): "));
  rb_str_concat(message, rb_hash_aref(document, rb_str_new2("errmsg")));

  return RSTRING_PTR(message);
}
