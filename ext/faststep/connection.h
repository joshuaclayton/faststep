#ifndef CONNECTION_H
#include <ruby.h>
#include "mongo.h"
#define CONNECTION_H
void faststep_connection_main();

static VALUE faststep_connection_init(VALUE, VALUE, VALUE);
static VALUE faststep_connection_new(VALUE, VALUE, VALUE);
static VALUE faststep_connection_connect(VALUE);
static VALUE faststep_connection_disconnect(VALUE);
static VALUE faststep_connection_connected(VALUE);

static void _faststep_connect_or_raise(mongo_connection*, mongo_connection_options*);
#endif
