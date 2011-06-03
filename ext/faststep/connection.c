#include "connection.h"
#include "faststep_defines.h"
#include "bson_ruby_conversion.h"

void faststep_connection_main() {
  rb_cFaststepConnection = rb_define_class_under(rb_mFaststep, "Connection", rb_cObject);

  rb_define_singleton_method(rb_cFaststepConnection, "new", faststep_connection_new, -1);

  rb_define_method(rb_cFaststepConnection, "initialize",  faststep_connection_init, -1);
  rb_define_method(rb_cFaststepConnection, "connect!",    faststep_connection_connect, 0);
  rb_define_method(rb_cFaststepConnection, "disconnect!", faststep_connection_disconnect, 0);
  rb_define_method(rb_cFaststepConnection, "connected?",  faststep_connection_connected, 0);
  rb_define_method(rb_cFaststepConnection, "master?",     faststep_connection_master, 0);

  return;
}

static VALUE faststep_connection_init(int argc, VALUE* argv, VALUE self) {
  VALUE host, port;

  rb_scan_args(argc, argv, "02", &host, &port);

  if(!RTEST(host)) { host = rb_str_new2("127.0.0.1"); }
  if(!RTEST(port)) { port = rb_const_get(rb_cFaststepConnection, rb_intern("DEFAULT_PORT")); }

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
  mongo_host_port* host_port = (mongo_host_port*)bson_malloc(sizeof(mongo_host_port));

  strcpy(host_port->host, RSTRING_PTR(rb_iv_get(self, "@host")));
  host_port->port = NUM2INT(rb_iv_get(self, "@port"));

  _faststep_connect_or_raise(GetFaststepConnection(self), host_port);

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

static void _faststep_connect_or_raise(mongo_connection* conn, mongo_host_port* host_port) {
  mongo_connect(conn, host_port->host, host_port->port);

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
