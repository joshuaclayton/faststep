#ifndef BSON_RUBY_CONVERSION_H
#include <ruby.h>
#include "bson.h"
#define BSON_RUBY_CONVERSION_H
bson* create_bson_from_ruby_hash(VALUE);
VALUE ruby_hash_from_bson(bson*);
VALUE ensure_document_ok(VALUE);
char* invalid_command_description(VALUE);
#endif
