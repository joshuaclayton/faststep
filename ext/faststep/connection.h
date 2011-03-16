#ifndef CONNECTION_H
#include <ruby.h>
#include "mongo.h"
#define CONNECTION_H
void faststep_connection_main();
void faststep_connection_free(mongo_connection*);
static VALUE faststep_connection_init(VALUE, VALUE, VALUE);
VALUE faststep_connection_new(VALUE, VALUE, VALUE);
VALUE faststep_connection_connect(VALUE);
VALUE faststep_connection_disconnect(VALUE);
VALUE faststep_connection_connected(VALUE);
#endif
