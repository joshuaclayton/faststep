#include "db.h"
#include "mongo.h"

void db_main(VALUE faststep) {
  VALUE FaststepDb = rb_define_class_under(faststep, "Db", rb_cObject);

  rb_define_attr(FaststepDb, "name", 1, 0);
  rb_define_attr(FaststepDb, "connection", 1, 0);

  rb_define_alias(FaststepDb, "[]", "collection");

  rb_define_method(FaststepDb, "initialize", db_init, 2);
  rb_define_method(FaststepDb, "drop", db_drop, 0);
}

VALUE db_init(VALUE self, VALUE name, VALUE connection) {
  rb_iv_set(self, "@name", name);
  rb_iv_set(self, "@connection", connection);

  return self;
}

VALUE db_drop(VALUE self) {
  mongo_connection* conn;
  Data_Get_Struct(rb_iv_get(self, "@connection"), mongo_connection, conn);

  if(mongo_cmd_drop_db(conn, RSTRING_PTR(rb_iv_get(self, "@name")))) {
    return Qtrue;
  } else {
    return Qfalse;
  }
}
