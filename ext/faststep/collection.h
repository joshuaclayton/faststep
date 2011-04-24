#ifndef COLLECTION_H
#include <ruby.h>
#include "mongo.h"
#include "bson.h"
#define COLLECTION_H

void  faststep_collection_main();
static VALUE faststep_collection_init(VALUE, const VALUE, const VALUE);
static VALUE faststep_collection_connection(const VALUE);
static VALUE faststep_collection_find(int, VALUE*, const VALUE);
static VALUE faststep_collection_find_one(int, VALUE*, const VALUE);
static VALUE faststep_collection_count(int, VALUE*, VALUE);
static VALUE faststep_collection_insert(int, VALUE*, const VALUE);
static VALUE faststep_collection_update(int, VALUE*, const VALUE);
static VALUE faststep_collection_remove(int, VALUE*, VALUE);
static VALUE faststep_collection_drop(const VALUE);
static VALUE faststep_collection_create_index(const VALUE, const VALUE);
VALUE faststep_collection_ns(const VALUE);

static void _faststep_collection_insert_one(mongo_connection*, const char*, const VALUE);
static void _faststep_collection_insert_batch(mongo_connection*, const char*, const VALUE);
mongo_connection* GetFaststepConnectionForCollection(const VALUE);
static void _faststep_collection_destroy(bson**, const int);
static VALUE _faststep_safe_operation(const VALUE, const VALUE);

void build_collection_ns(char*, const char*, const char*);
static char* _ivar_name(const VALUE);
#endif
