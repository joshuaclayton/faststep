#include "collection.h"
#include "mongo.h"
#include "bson_ruby_conversion.h"
#include "faststep_defines.h"
#include <string.h>

void collection_main() {
  rb_cFaststepCollection = rb_define_class_under(rb_mFaststep, "Collection", rb_cObject);

  rb_define_attr(rb_cFaststepCollection, "name", 1, 0);

  rb_define_method(rb_cFaststepCollection, "initialize",   collection_init, 2);
  rb_define_method(rb_cFaststepCollection, "ns",           collection_ns, 0);
  rb_define_method(rb_cFaststepCollection, "count",        collection_count, -1);
  rb_define_method(rb_cFaststepCollection, "insert",       collection_insert, 1);
  rb_define_method(rb_cFaststepCollection, "update",       collection_update, 2);
  rb_define_method(rb_cFaststepCollection, "drop",         collection_drop, 0);
  rb_define_method(rb_cFaststepCollection, "create_index", collection_create_index, 1);
  return;
}

VALUE collection_init(VALUE self, VALUE name, VALUE database) {
  rb_iv_set(self, "@name", name);
  rb_iv_set(self, "@db", database);

  return self;
}

VALUE collection_ns(VALUE self) {
  VALUE db = rb_iv_get(self, "@db");

  char ns[255] = "";
  build_collection_ns(ns, _name(db), _name(self));

  return rb_str_new2(ns);
}

char* _name(VALUE obj) {
  return RSTRING_PTR(rb_iv_get(obj, "@name"));
}

mongo_connection* database_connection(VALUE database) {
  mongo_connection* conn;
  Data_Get_Struct(rb_iv_get(database, "@connection"), mongo_connection, conn);
  return conn;
}

VALUE collection_count(int argc, VALUE* argv, VALUE self) {
  VALUE query;
  rb_scan_args(argc, argv, "01", &query);

  bson* bson_query = bson_malloc(sizeof(bson));
  init_bson_from_ruby_hash(bson_query, query);

  VALUE db = rb_iv_get(self, "@db");

  VALUE result = ULL2NUM(mongo_count(database_connection(db), _name(db), _name(self), bson_query));
  bson_destroy(bson_query);
  return result;
}

void build_collection_ns(char* ns, char* database, char* collection) {
  strcat(ns, database);
  strcat(ns, ".");
  strcat(ns, collection);

  return;
}

VALUE collection_insert(VALUE self, VALUE documents) {
  VALUE db = rb_iv_get(self, "@db");
  mongo_connection* conn = database_connection(db);

  VALUE ns = collection_ns(self);

  if(TYPE(documents) == T_ARRAY) {
    int document_count = RARRAY_LEN(documents);
    int iterator;

    for(iterator = 0; iterator < document_count; iterator++) {
      VALUE document = rb_ary_entry(documents, iterator);
      collection_insert_one(conn, RSTRING_PTR(ns), document);
    }
  } else {
    collection_insert_one(conn, RSTRING_PTR(ns), documents);
  }

  return Qtrue;
}

void collection_insert_one(mongo_connection* conn, char* ns, VALUE document) {
  bson* bson_document = bson_malloc(sizeof(bson));
  init_bson_from_ruby_hash(bson_document, document);

  mongo_insert(conn, ns, bson_document);

  bson_destroy(bson_document);
}

void collection_insert_batch(mongo_connection* conn, char* ns, VALUE documents) {
  int document_count = RARRAY_LEN(documents);
  int iterator;

  bson** bson_documents = (bson**)bson_malloc(sizeof(bson*) * document_count);

  for(iterator = 0; iterator < document_count; iterator++) {
    bson* bson_document = bson_malloc(sizeof(bson));

    VALUE document = rb_ary_entry(documents, iterator);

    init_bson_from_ruby_hash(bson_document, document);
    bson_documents[iterator] = bson_document;
  }

  mongo_insert_batch(conn, ns, bson_documents, document_count);

  for(iterator = 0; iterator < document_count; iterator++) {
    bson_destroy(bson_documents[iterator]);
  }
}

VALUE collection_update(VALUE self, VALUE query, VALUE operations) {
  VALUE db = rb_iv_get(self, "@db");
  mongo_connection* conn = database_connection(db);

  bson* bson_query      = bson_malloc(sizeof(bson));
  bson* bson_operations = bson_malloc(sizeof(bson));

  init_bson_from_ruby_hash(bson_query, query);
  init_bson_from_ruby_hash(bson_operations, operations);

  mongo_update(conn, RSTRING_PTR(collection_ns(self)), bson_query, bson_operations, MONGO_UPDATE_MULTI);

  bson_destroy(bson_query);
  bson_destroy(bson_operations);

  return Qtrue;
}

VALUE collection_drop(VALUE self) {
  VALUE db = rb_iv_get(self, "@db");
  mongo_connection* conn = database_connection(db);
  if(mongo_cmd_drop_collection(conn, _name(db), _name(self), NULL)) {
    return Qtrue;
  } else {
    return Qfalse;
  }
}

VALUE collection_create_index(VALUE self, VALUE indexes) {
  VALUE db = rb_iv_get(self, "@db");
  mongo_connection* conn = database_connection(db);

  bson* bson_indexes = bson_malloc(sizeof(bson));
  bson* bson_out     = bson_malloc(sizeof(bson));

  init_bson_from_ruby_hash(bson_indexes, indexes);

  int options = 0;
  bson_bool_t result = mongo_create_index(conn, RSTRING_PTR(collection_ns(self)), bson_indexes, options, bson_out);
  bson_destroy(bson_indexes);
  bson_destroy(bson_out);

  return result ? Qtrue : Qfalse;
}
