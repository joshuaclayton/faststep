#ifndef COLLECTION_H
#include <ruby.h>
#include "mongo.h"
#include "bson.h"
#define COLLECTION_H

void  faststep_collection_main();
VALUE faststep_collection_init(VALUE, VALUE, VALUE);
VALUE faststep_collection_count(int, VALUE*, VALUE);
VALUE faststep_collection_insert(VALUE, VALUE);
VALUE faststep_collection_update(VALUE, VALUE, VALUE);
VALUE faststep_collection_drop(VALUE);
VALUE faststep_collection_create_index(VALUE, VALUE);

void faststep_collection_insert_one(mongo_connection*, char*, VALUE);
void faststep_collection_insert_batch(mongo_connection*, char*, VALUE);

VALUE faststep_collection_ns(VALUE);
void build_collection_ns(char*, char*, char*);
void init_bson_from_ruby_hash(bson*, VALUE);
VALUE ruby_hash_from_bson(bson*);
char* _name(VALUE);
mongo_connection* faststep_database_connection(VALUE);
#endif
