#ifndef BSON_RUBY_CONVERSION_H
#include <ruby.h>
#include "bson.h"
#define BSON_RUBY_CONVERSION_H
void init_bson_from_ruby_hash(bson*, VALUE);
VALUE ruby_hash_from_bson(bson*);
VALUE ensure_document_ok(VALUE, int);
char* invalid_command_description(VALUE);
#endif