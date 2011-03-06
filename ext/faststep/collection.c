#include "collection.h"
#include "mongo.h"

void collection_main(VALUE faststep) {
  VALUE FaststepCollection = rb_define_class_under(faststep, "Collection", rb_cObject);

  rb_define_attr(FaststepCollection, "name", 1, 0);

  rb_define_method(FaststepCollection, "initialize", collection_init, 2);
  rb_define_method(FaststepCollection, "count",      collection_count, 0);
  return;
}

VALUE collection_init(VALUE self, VALUE name, VALUE db) {
  rb_iv_set(self, "@name", name);
  rb_iv_set(self, "@db", db);

  return self;
}

VALUE collection_count(VALUE self) {
  mongo_connection* conn;
  Data_Get_Struct(rb_iv_get(self, "@db"), mongo_connection, conn);

  bson empty[1];
  bson_empty(empty);

  return ULL2NUM(mongo_count(conn, "test", RSTRING_PTR(rb_iv_get(self, "@name")), empty));
}
