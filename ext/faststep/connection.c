#include "connection.h"
#include "faststep_defines.h"
#include "bson_ruby_conversion.h"

void faststep_connection_main() {
  rb_cFaststepConnection = rb_define_class_under(rb_mFaststep, "Connection", rb_cObject);
  rb_define_const(rb_cFaststepConnection, "DEFAULT_PORT", INT2FIX(27017));

  rb_define_attr(rb_cFaststepConnection, "host", 1, 0);
  rb_define_attr(rb_cFaststepConnection, "port", 1, 0);

  rb_define_singleton_method(rb_cFaststepConnection, "new", faststep_connection_new, -1);

  rb_define_method(rb_cFaststepConnection, "initialize",  faststep_connection_init, -1);
  rb_define_method(rb_cFaststepConnection, "connect!",    faststep_connection_connect, 0);
  rb_define_method(rb_cFaststepConnection, "disconnect!", faststep_connection_disconnect, 0);
  rb_define_method(rb_cFaststepConnection, "connected?",  faststep_connection_connected, 0);
  rb_define_method(rb_cFaststepConnection, "master?",     faststep_connection_master, 0);
  rb_define_method(rb_cFaststepConnection, "db",          faststep_connection_db, 1);

  return;
}

/* static VALUE faststep_connection_init(VALUE self, const VALUE host, const VALUE port) { */
static VALUE faststep_connection_init(int argc, VALUE* argv, VALUE self) {
  VALUE host, port;

  rb_scan_args(argc, argv, "02", &host, &port);

  if(!RTEST(host)) { host = rb_str_new2("127.0.0.1"); }
  if(!RTEST(port)) { port = INT2NUM(27017); }

  rb_iv_set(self, "@host", host);
  rb_iv_set(self, "@port", port);

  faststep_connection_connect(self);

  return self;
}

static VALUE faststep_connection_new(int argc, VALUE* argv, VALUE class) {
  mongo_connection* conn = (mongo_connection*)bson_malloc(sizeof(mongo_connection));

  VALUE tdata = Data_Wrap_Struct(class, NULL, mongo_destroy, conn);

  rb_obj_call_init(tdata, argc, argv);

  return tdata;
}

static VALUE faststep_connection_connect(VALUE self) {
  mongo_connection_options* options = (mongo_connection_options*)bson_malloc(sizeof(mongo_connection_options));

  strcpy(options->host, RSTRING_PTR(rb_iv_get(self, "@host")));
  options->port = NUM2INT(rb_iv_get(self, "@port"));

  _faststep_connect_or_raise(GetFaststepConnection(self), options);

  return Qnil;
}

static VALUE faststep_connection_disconnect(const VALUE self) {
  mongo_disconnect(GetFaststepConnection(self));

  return Qnil;
}

static VALUE faststep_connection_connected(const VALUE self) {
  return bool_to_ruby(GetFaststepConnection(self)->connected);
}

static VALUE faststep_connection_master(const VALUE self) {
  return bool_to_ruby(mongo_cmd_ismaster(GetFaststepConnection(self), NULL));
}

static VALUE faststep_connection_db(const VALUE self, const VALUE database_name) {
  return rb_funcall(rb_cFaststepDb, rb_intern("new"), 2, database_name, self);
}

static void _faststep_connect_or_raise(mongo_connection* conn, mongo_connection_options* options) {
  mongo_connect(conn, options);

  if(conn->connected == 0) {
    mongo_destroy(conn);
    rb_raise(rb_eFaststepConnectionFailure, "unable to connect to Mongo");
  }

  return;
}

mongo_connection* GetFaststepConnection(const VALUE object) {
  mongo_connection* conn;
  Data_Get_Struct(object, mongo_connection, conn);
  return conn;
}
