#include "db.h"
#include "mongo.h"
#include "bson.h"
#include "connection.h"
#include "collection.h"
#include "bson_ruby_conversion.h"
#include "faststep_defines.h"

void faststep_db_main() {
  rb_cFaststepDb = rb_define_class_under(rb_mFaststep, "Db", rb_cObject);

  rb_define_method(rb_cFaststepDb, "drop",           faststep_db_drop, 0);
  rb_define_method(rb_cFaststepDb, "command",        faststep_db_command, 1);
  rb_define_method(rb_cFaststepDb, "get_last_error", faststep_db_get_last_error, 0);
}

static VALUE faststep_db_drop(VALUE self) {
  mongo* conn = GetFaststepConnection(rb_iv_get(self, "@connection"));

  return bool_to_ruby(mongo_cmd_drop_db(conn, RSTRING_PTR(rb_iv_get(self, "@name"))));
}

static VALUE faststep_db_command(VALUE self, VALUE command) {
  bson* result       = (bson*)bson_malloc(sizeof(bson));
  bson* bson_command = create_bson_from_ruby_hash(command);

  VALUE ns = build_collection_ns(rb_iv_get(self, "@name"), rb_str_new2("$cmd"));

  mongo_find_one(GetFaststepConnection(rb_iv_get(self, "@connection")),
                 RSTRING_PTR(ns),
                 bson_command,
                 NULL,
                 result);

  VALUE hash = ruby_hash_from_bson(result);

  bson_destroy(result);
  free(result);
  bson_destroy(bson_command);
  free(bson_command);

  ensure_document_ok(hash);

  return hash;
}

static VALUE faststep_db_get_last_error(VALUE self) {
  bson* result = (bson*)bson_malloc(sizeof(bson));

  mongo_cmd_get_last_error(GetFaststepConnection(rb_iv_get(self, "@connection")),
                           RSTRING_PTR(rb_iv_get(self, "@name")),
                           result);

  VALUE last_error = ruby_hash_from_bson(result);
  bson_destroy(result);
  free(result);

  return last_error;
}
