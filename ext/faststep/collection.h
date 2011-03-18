#ifndef COLLECTION_H
#include <ruby.h>
#include "mongo.h"
#include "bson.h"
#define COLLECTION_H

void  faststep_collection_main();
static VALUE faststep_collection_init(VALUE, VALUE, VALUE);
static VALUE faststep_collection_count(int, VALUE*, VALUE);
static VALUE faststep_collection_insert(VALUE, VALUE);
static VALUE faststep_collection_update(VALUE, VALUE, VALUE);
static VALUE faststep_collection_remove(int, VALUE*, VALUE);
static VALUE faststep_collection_drop(VALUE);
static VALUE faststep_collection_create_index(VALUE, VALUE);
VALUE faststep_collection_ns(VALUE);

static void _faststep_collection_insert_one(mongo_connection*, char*, VALUE);
static void _faststep_collection_insert_batch(mongo_connection*, char*, VALUE);
mongo_connection* GetFaststepConnectionForCollection(VALUE);
static void _faststep_collection_destroy(bson**, int);

void build_collection_ns(char*, char*, char*);
static char* _ivar_name(VALUE);
#endif
