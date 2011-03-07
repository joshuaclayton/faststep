#include "collection.h"
#include "mongo.h"

void collection_main(VALUE faststep) {
  VALUE FaststepCollection = rb_define_class_under(faststep, "Collection", rb_cObject);

  rb_define_attr(FaststepCollection, "name", 1, 0);

  rb_define_method(FaststepCollection, "initialize", collection_init, 2);
  rb_define_method(FaststepCollection, "count",      collection_count, -1);
  return;
}

VALUE collection_init(VALUE self, VALUE name, VALUE db) {
  rb_iv_set(self, "@name", name);
  rb_iv_set(self, "@db", db);

  return self;
}

void init_bson_from_ruby_hash(bson* bson, VALUE hash) {
  VALUE rb_bson = rb_const_get(rb_cObject, rb_intern("BSON"));
  VALUE byte_buffer = rb_funcall(rb_bson, rb_intern("serialize"), 3, hash, Qfalse, Qfalse);
  VALUE query = rb_funcall(byte_buffer, rb_intern("to_s"), 0);
  bson_init(bson, RSTRING_PTR(query), 1);
  return;
}

VALUE collection_count(int argc, VALUE* argv, VALUE self) {
  mongo_connection* conn;
  Data_Get_Struct(rb_iv_get(self, "@db"), mongo_connection, conn);

  bson* bson_query = malloc(sizeof(bson));

  VALUE query;
  rb_scan_args(argc, argv, "01", &query);

  if(NIL_P(query)) {
    bson_empty(bson_query);
  } else {
    init_bson_from_ruby_hash(bson_query, query);
  }

  return ULL2NUM(mongo_count(conn, "test", RSTRING_PTR(rb_iv_get(self, "@name")), bson_query));
}
