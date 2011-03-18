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
  rb_define_method(rb_cFaststepCollection, "remove",       faststep_collection_remove, -1);
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
  mongo_connection* conn = GetFaststepConnectionForCollection(self);

  VALUE ns = faststep_collection_ns(self);

  if(TYPE(documents) == T_ARRAY) {
    _faststep_collection_insert_batch(conn, RSTRING_PTR(ns), documents);
  } else {
    _faststep_collection_insert_one(conn, RSTRING_PTR(ns), documents);
  }

  return Qtrue;
}

static VALUE faststep_collection_update(VALUE self, VALUE query, VALUE operations) {
  bson* bson_query      = create_bson_from_ruby_hash(query);
  bson* bson_operations = create_bson_from_ruby_hash(operations);

  mongo_update(GetFaststepConnectionForCollection(self),
               RSTRING_PTR(faststep_collection_ns(self)),
               bson_query,
               bson_operations,
               MONGO_UPDATE_MULTI);

  bson_destroy(bson_query);
  bson_destroy(bson_operations);

  return Qtrue;
}

static VALUE faststep_collection_remove(int argc, VALUE* argv, VALUE self) {
  VALUE query;
  rb_scan_args(argc, argv, "01", &query);

  bson* bson_query = create_bson_from_ruby_hash(query);

  mongo_remove(GetFaststepConnectionForCollection(self),
               RSTRING_PTR(faststep_collection_ns(self)),
               bson_query);
  bson_destroy(bson_query);
  return Qnil;
}

static VALUE faststep_collection_drop(VALUE self) {
  bson_bool_t result = mongo_cmd_drop_collection(GetFaststepConnectionForCollection(self),
                                                 _ivar_name(rb_iv_get(self, "@db")),
                                                 _ivar_name(self),
                                                 NULL);

  return bool_to_ruby(result);
}

static VALUE faststep_collection_create_index(VALUE self, VALUE indexes) {
  bson* bson_indexes = create_bson_from_ruby_hash(indexes);

  bson_bool_t result = mongo_create_index(GetFaststepConnectionForCollection(self),
                                          RSTRING_PTR(faststep_collection_ns(self)),
                                          bson_indexes,
                                          0,
                                          NULL);
  bson_destroy(bson_indexes);

  return bool_to_ruby(result);
}

static void _faststep_collection_insert_one(mongo_connection* conn, char* ns, VALUE document) {
  bson* bson_document = create_bson_from_ruby_hash(document);
  mongo_insert(conn, ns, bson_document);
  bson_destroy(bson_document);
}

static void _faststep_collection_insert_batch(mongo_connection* conn, char* ns, VALUE documents) {
  int total_document_count = RARRAY_LEN(documents);
  bson** bson_documents = (bson**)malloc(sizeof(bson*) * total_document_count);

  int chunk_size = 1024*1024*16;
  int sent_document_count    = 0,
      current_bson_size      = 0,
      current_document_count = 0,
      iterator;

  for(iterator = 0; iterator < total_document_count; iterator++) {
    int current_document_array_index = sent_document_count + current_document_count;
    bson* document = create_bson_from_ruby_hash(rb_ary_entry(documents, current_document_array_index));

    if(bson_size(document) + current_bson_size > chunk_size) {
      mongo_insert_batch(conn, ns, bson_documents, current_document_count);
      _faststep_collection_destroy(bson_documents, current_document_count);

      sent_document_count += current_document_count;
      total_document_count -= current_document_count;

      current_bson_size = 0;
      current_document_count = 0;
      iterator = 0;
    }

    bson_documents[iterator] = document;
    current_document_count += 1;
    current_bson_size += bson_size(document);
  }

  if(current_document_count > 0) {
    mongo_insert_batch(conn, ns, bson_documents, current_document_count);
    _faststep_collection_destroy(bson_documents, current_document_count);
  }
}

static void _faststep_collection_destroy(bson** bson_documents, int document_count) {
  int iterator;
  for(iterator = 0; iterator < document_count; iterator++) {
    bson_destroy(bson_documents[iterator]);
  }
  return;
}

static char* _ivar_name(VALUE obj) {
  return RSTRING_PTR(rb_iv_get(obj, "@name"));
}

mongo_connection* GetFaststepConnectionForCollection(VALUE collection) {
  VALUE db         = rb_iv_get(collection, "@db");
  VALUE connection = rb_iv_get(db, "@connection");

  return GetFaststepConnection(connection);
}
