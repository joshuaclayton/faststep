#ifndef BSON_RUBY_CONVERSION_H
#include <ruby.h>
#include "bson.h"
#define BSON_RUBY_CONVERSION_H
bson* create_bson_from_ruby_hash(VALUE);
VALUE ruby_hash_from_bson(bson*);
VALUE ruby_array_to_bson_ordered_hash(VALUE);
bson* bson_from_ruby_array(VALUE);
VALUE ensure_document_ok(VALUE);
VALUE bool_to_ruby(bson_bool_t);
static char* _invalid_command_description(VALUE);
static VALUE _map_assoc_ary_to_key_value_pair(VALUE, VALUE);
#endif
