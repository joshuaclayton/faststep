#ifndef CONNECTION_H
#include <ruby.h>
#include "mongo.h"
#define CONNECTION_H
void faststep_connection_main();

static VALUE faststep_connection_init(VALUE, const VALUE, const VALUE);
static VALUE faststep_connection_new(VALUE, const VALUE, const VALUE);
static VALUE faststep_connection_connect(VALUE);
static VALUE faststep_connection_disconnect(const VALUE);
static VALUE faststep_connection_connected(const VALUE);
static VALUE faststep_connection_master(const VALUE);

mongo_connection* GetFaststepConnection(const VALUE);

static void _faststep_connect_or_raise(mongo_connection*, mongo_connection_options*);
#endif
