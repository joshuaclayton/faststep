#ifndef COLLECTION_H
#include <ruby.h>
#include "mongo.h"
#include "bson.h"
#define COLLECTION_H

void collection_main(VALUE);
VALUE collection_init(VALUE, VALUE, VALUE);
VALUE collection_count(int, VALUE*, VALUE);
VALUE collection_insert(VALUE, VALUE);
VALUE collection_update(VALUE, VALUE, VALUE);
VALUE collection_ns(VALUE);
void build_collection_ns(char*, char*, char*);
void init_bson_from_ruby_hash(bson*, VALUE);
VALUE ruby_hash_from_bson(bson*);
char* _name(VALUE);
mongo_connection* database_connection(VALUE);
#endif
