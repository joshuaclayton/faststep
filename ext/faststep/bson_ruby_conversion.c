#include "bson_ruby_conversion.h"
#include "exceptions.h"

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

VALUE ensure_document_ok(VALUE document, int raise_unless_ok) {
  VALUE ok_value = rb_hash_aref(document, rb_str_new2("ok"));

  if(raise_unless_ok && !NUM2INT(ok_value)) {
    RaiseFaststepException("OperationFailure", invalid_command_description(document));
  }
}

char* invalid_command_description(VALUE document) {
  VALUE message = rb_str_new2("Invalid command (");
  rb_str_concat(message, rb_funcall(rb_hash_aref(document, rb_str_new2("bad cmd")), rb_intern("inspect"), 0));
  rb_str_concat(message, rb_str_new2("): "));
  rb_str_concat(message, rb_hash_aref(document, rb_str_new2("errmsg")));

  return RSTRING_PTR(message);
}
