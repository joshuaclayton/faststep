#ifndef BSON_RUBY_CONVERSION_H
#include <ruby.h>
#include "bson.h"
#define BSON_RUBY_CONVERSION_H
bson* create_bson_from_ruby_hash(VALUE);
VALUE ruby_hash_from_bson(bson*);
bson* bson_from_ruby_array(VALUE);
VALUE ensure_document_ok(VALUE);
VALUE bool_to_ruby(bson_bool_t);
static char* _invalid_command_description(VALUE);
#endif
