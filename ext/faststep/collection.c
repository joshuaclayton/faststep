#include "collection.h"
#include "connection.h"
#include "bson_ruby_conversion.h"
#include "faststep_defines.h"
#include <string.h>

void faststep_collection_main() {
  rb_cFaststepCollection = rb_define_class_under(rb_mFaststep, "Collection", rb_cObject);

  rb_define_attr(rb_cFaststepCollection, "name", 1, 0);

  rb_define_method(rb_cFaststepCollection, "initialize",   faststep_collection_init, 2);
  rb_define_method(rb_cFaststepCollection, "ns",           faststep_collection_ns, 0);
  rb_define_method(rb_cFaststepCollection, "count",        faststep_collection_count, -1);
  rb_define_method(rb_cFaststepCollection, "insert",       faststep_collection_insert, 1);
  rb_define_method(rb_cFaststepCollection, "update",       faststep_collection_update, 2);
  rb_define_method(rb_cFaststepCollection, "drop",         faststep_collection_drop, 0);
  rb_define_method(rb_cFaststepCollection, "create_index", faststep_collection_create_index, 1);
  return;
}

static VALUE faststep_collection_init(VALUE self, VALUE name, VALUE database) {
  rb_iv_set(self, "@name", name);
  rb_iv_set(self, "@db", database);

  return self;
}

VALUE faststep_collection_ns(VALUE self) {
  VALUE db = rb_iv_get(self, "@db");

  char ns[255] = "";
  build_collection_ns(ns, _ivar_name(db), _ivar_name(self));

  return rb_str_new2(ns);
}

static VALUE faststep_collection_count(int argc, VALUE* argv, VALUE self) {
  VALUE query;
  rb_scan_args(argc, argv, "01", &query);

  bson* bson_query = create_bson_from_ruby_hash(query);

  int64_t count = mongo_count(GetFaststepConnectionForCollection(self),
                              _ivar_name(rb_iv_get(self, "@db")),
                              _ivar_name(self),
                              bson_query);

  bson_destroy(bson_query);
  return ULL2NUM(count);
}

void build_collection_ns(char* ns, char* database, char* collection) {
  strcat(ns, database);
  strcat(ns, ".");
  strcat(ns, collection);

  return;
}

static VALUE faststep_collection_insert(VALUE self, VALUE documents) {
  VALUE db = rb_iv_get(self, "@db");
  mongo_connection* conn = GetFaststepConnectionForCollection(self);

  VALUE ns = faststep_collection_ns(self);

  if(TYPE(documents) == T_ARRAY) {
    int document_count = RARRAY_LEN(documents);
    int iterator;

    for(iterator = 0; iterator < document_count; iterator++) {
      VALUE document = rb_ary_entry(documents, iterator);
      _faststep_collection_insert_one(conn, RSTRING_PTR(ns), document);
    }
  } else {
    _faststep_collection_insert_one(conn, RSTRING_PTR(ns), documents);
  }

  return Qtrue;
}

static VALUE faststep_collection_update(VALUE self, VALUE query, VALUE operations) {
  VALUE db = rb_iv_get(self, "@db");
  mongo_connection* conn = GetFaststepConnectionForCollection(self);

  bson* bson_query      = create_bson_from_ruby_hash(query);
  bson* bson_operations = create_bson_from_ruby_hash(operations);

  mongo_update(conn,
               RSTRING_PTR(faststep_collection_ns(self)),
               bson_query,
               bson_operations,
               MONGO_UPDATE_MULTI);

  bson_destroy(bson_query);
  bson_destroy(bson_operations);

  return Qtrue;
}

static VALUE faststep_collection_drop(VALUE self) {
  VALUE db = rb_iv_get(self, "@db");
  mongo_connection* conn = GetFaststepConnectionForCollection(self);

  if(mongo_cmd_drop_collection(conn, _ivar_name(db), _ivar_name(self), NULL)) {
    return Qtrue;
  } else {
    return Qfalse;
  }
}

static VALUE faststep_collection_create_index(VALUE self, VALUE indexes) {
  VALUE db = rb_iv_get(self, "@db");
  mongo_connection* conn = GetFaststepConnectionForCollection(self);

  bson* bson_indexes = create_bson_from_ruby_hash(indexes);
  bson* bson_out     = bson_malloc(sizeof(bson));

  int options = 0;
  bson_bool_t result = mongo_create_index(conn,
                                          RSTRING_PTR(faststep_collection_ns(self)),
                                          bson_indexes,
                                          options,
                                          bson_out);
  bson_destroy(bson_indexes);
  bson_destroy(bson_out);

  return result ? Qtrue : Qfalse;
}

static void _faststep_collection_insert_one(mongo_connection* conn, char* ns, VALUE document) {
  bson* bson_document = create_bson_from_ruby_hash(document);
  mongo_insert(conn, ns, bson_document);
  bson_destroy(bson_document);
}

static void _faststep_collection_insert_batch(mongo_connection* conn, char* ns, VALUE documents) {
  int document_count = RARRAY_LEN(documents);
  int iterator;

  bson** bson_documents = (bson**)bson_malloc(sizeof(bson*) * document_count);

  for(iterator = 0; iterator < document_count; iterator++) {
    VALUE document = rb_ary_entry(documents, iterator);

    bson_documents[iterator] = create_bson_from_ruby_hash(document);
  }

  mongo_insert_batch(conn, ns, bson_documents, document_count);

  for(iterator = 0; iterator < document_count; iterator++) {
    bson_destroy(bson_documents[iterator]);
  }
}

static char* _ivar_name(VALUE obj) {
  return RSTRING_PTR(rb_iv_get(obj, "@name"));
}

mongo_connection* GetFaststepConnectionForCollection(VALUE collection) {
  VALUE db         = rb_iv_get(collection, "@db");
  VALUE connection = rb_iv_get(db, "@connection");

  return GetFaststepConnection(connection);
}
