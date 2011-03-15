#ifndef CONNECTION_H
#include <ruby.h>
#include "mongo.h"
#define CONNECTION_H
void connection_main();
void connection_free(mongo_connection*);
static VALUE connection_init(VALUE, VALUE, VALUE);
VALUE connection_new(VALUE, VALUE, VALUE);
VALUE connection_connect(VALUE);
VALUE connection_disconnect(VALUE);
VALUE connection_connected(VALUE);
#endif
