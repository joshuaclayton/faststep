#ifndef COLLECTION_H
#include <ruby.h>
#define COLLECTION_H

void collection_main(VALUE);
VALUE collection_init(VALUE, VALUE, VALUE);
VALUE collection_count(VALUE);
#endif
