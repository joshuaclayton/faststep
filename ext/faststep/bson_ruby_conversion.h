#ifndef BSON_RUBY_CONVERSION_H
#include <ruby.h>
#include "bson.h"
#define BSON_RUBY_CONVERSION_H
bson* create_bson_from_ruby_hash(const VALUE);
bson* bson_from_ruby_array(const VALUE);
VALUE ruby_array_to_bson_ordered_hash(const VALUE);
VALUE ruby_hash_from_bson(const bson*);
VALUE bool_to_ruby(const bson_bool_t);
VALUE ensure_document_ok(const VALUE);
static VALUE _invalid_command_description(const VALUE);
static VALUE _map_assoc_ary_to_key_value_pair(const VALUE, VALUE);
#endif
