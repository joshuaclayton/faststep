#include "db.h"
#include "mongo.h"
#include "bson.h"
#include "collection.h"
#include "bson_ruby_conversion.h"
#include "faststep_defines.h"

void db_main() {
  rb_cFaststepDb = rb_define_class_under(rb_mFaststep, "Db", rb_cObject);

  rb_define_attr(rb_cFaststepDb, "name", 1, 0);
  rb_define_attr(rb_cFaststepDb, "connection", 1, 0);

  rb_define_alias(rb_cFaststepDb, "[]", "collection");

  rb_define_method(rb_cFaststepDb, "initialize", db_init, 2);
  rb_define_method(rb_cFaststepDb, "drop",       db_drop, 0);
  rb_define_method(rb_cFaststepDb, "command",    db_command, 1);
}

VALUE db_init(VALUE self, VALUE name, VALUE connection) {
  rb_iv_set(self, "@name", name);
  rb_iv_set(self, "@connection", connection);

  return self;
}

VALUE db_drop(VALUE self) {
  mongo_connection* conn;
  Data_Get_Struct(rb_iv_get(self, "@connection"), mongo_connection, conn);

  int result = mongo_cmd_drop_db(conn, RSTRING_PTR(rb_iv_get(self, "@name")));
  return result ? Qtrue : Qfalse;
}

VALUE db_command(VALUE self, VALUE command) {
  mongo_connection* conn;
  Data_Get_Struct(rb_iv_get(self, "@connection"), mongo_connection, conn);

  bson* result = bson_malloc(sizeof(bson));
  bson_init(result, "", 1);

  bson* bson_command = bson_malloc(sizeof(bson));
  init_bson_from_ruby_hash(bson_command, command);

  char ns[500] = "";
  build_collection_ns(ns, RSTRING_PTR(rb_iv_get(self, "@name")), "$cmd");

  mongo_find_one(conn, ns, bson_command, NULL, result);

  bson_destroy(bson_command);

  VALUE hash = ruby_hash_from_bson(result);
  bson_destroy(result);

  ensure_document_ok(hash);

  return hash;
}
