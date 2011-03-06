#include "connection.h"

void connection_main(VALUE faststep) {
  VALUE FaststepConnection = rb_define_class_under(faststep, "Connection", rb_cObject);

  rb_define_attr(FaststepConnection, "host", 1, 0);
  rb_define_attr(FaststepConnection, "port", 1, 0);

  rb_define_singleton_method(FaststepConnection, "new",  connection_new, 2);

  rb_define_method(FaststepConnection, "initialize",  connection_init, 2);
  rb_define_method(FaststepConnection, "connect!",    connection_connect, 0);
  rb_define_method(FaststepConnection, "disconnect!", connection_disconnect, 0);
  rb_define_method(FaststepConnection, "connected?",  connection_connected, 0);

  return;
}

void connection_free(mongo_connection* conn) {
  mongo_destroy(conn);
  return;
}

static VALUE connection_init(VALUE self, VALUE host, VALUE port) {
  rb_iv_set(self, "@host", host);
  rb_iv_set(self, "@port", port);

  connection_connect(self);

  return self;
}

VALUE connection_new(VALUE class, VALUE host, VALUE port) {
  mongo_connection* conn = malloc(sizeof(mongo_connection));

  conn->connected = 0;

  VALUE tdata = Data_Wrap_Struct(class, NULL, connection_free, conn);

  VALUE argv[2];
  argv[0] = host;
  argv[1] = port;

  rb_obj_call_init(tdata, 2, argv);

  return tdata;
}

VALUE connection_connect(VALUE self) {
  mongo_connection* conn;
  Data_Get_Struct(self, mongo_connection, conn);

  mongo_connection_options options[1];

  strcpy(options->host, RSTRING_PTR(rb_iv_get(self, "@host")));
  options->port = NUM2INT(rb_iv_get(self, "@port"));

  mongo_connect(conn, options);

  return Qnil;
}

VALUE connection_disconnect(VALUE self) {
  mongo_connection* conn;
  Data_Get_Struct(self, mongo_connection, conn);

  mongo_disconnect(conn);
  return Qnil;
}

VALUE connection_connected(VALUE self) {
  mongo_connection* conn;
  Data_Get_Struct(self, mongo_connection, conn);

  if(conn->connected) {
    return Qtrue;
  } else {
    return Qfalse;
  }
}
