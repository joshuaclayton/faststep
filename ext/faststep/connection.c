#include "connection.h"
#include "faststep_defines.h"

void faststep_connection_main() {
  rb_cFaststepConnection = rb_define_class_under(rb_mFaststep, "Connection", rb_cObject);

  rb_define_attr(rb_cFaststepConnection, "host", 1, 0);
  rb_define_attr(rb_cFaststepConnection, "port", 1, 0);

  rb_define_singleton_method(rb_cFaststepConnection, "new", faststep_connection_new, 2);

  rb_define_method(rb_cFaststepConnection, "initialize",  faststep_connection_init, 2);
  rb_define_method(rb_cFaststepConnection, "connect!",    faststep_connection_connect, 0);
  rb_define_method(rb_cFaststepConnection, "disconnect!", faststep_connection_disconnect, 0);
  rb_define_method(rb_cFaststepConnection, "connected?",  faststep_connection_connected, 0);

  return;
}

static VALUE faststep_connection_init(VALUE self, VALUE host, VALUE port) {
  rb_iv_set(self, "@host", host);
  rb_iv_set(self, "@port", port);

  faststep_connection_connect(self);

  return self;
}

static VALUE faststep_connection_new(VALUE class, VALUE host, VALUE port) {
  mongo_connection* conn = bson_malloc(sizeof(mongo_connection));

  VALUE tdata = Data_Wrap_Struct(class, NULL, mongo_destroy, conn);

  VALUE argv[2];
  argv[0] = host;
  argv[1] = port;

  rb_obj_call_init(tdata, 2, argv);

  return tdata;
}

static VALUE faststep_connection_connect(VALUE self) {
  mongo_connection_options* options = bson_malloc(sizeof(mongo_connection_options));

  strcpy(options->host, RSTRING_PTR(rb_iv_get(self, "@host")));
  options->port = NUM2INT(rb_iv_get(self, "@port"));

  _faststep_connect_or_raise(GetFaststepConnection(self), options);

  return Qnil;
}

static VALUE faststep_connection_disconnect(VALUE self) {
  mongo_disconnect(GetFaststepConnection(self));

  return Qnil;
}

static VALUE faststep_connection_connected(VALUE self) {
  return GetFaststepConnection(self)->connected ? Qtrue : Qfalse;
}

static void _faststep_connect_or_raise(mongo_connection* conn, mongo_connection_options* options) {
  mongo_connect(conn, options);

  if(conn->connected == 0) {
    mongo_destroy(conn);
    rb_raise(rb_cFaststepConnectionFailure, "unable to connect to Mongo");
  }

  return;
}

mongo_connection* GetFaststepConnection(VALUE object) {
  mongo_connection* conn;
  Data_Get_Struct(object, mongo_connection, conn);
  return conn;
}
