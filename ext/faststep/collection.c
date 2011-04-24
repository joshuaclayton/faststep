#include "collection.h"
#include "connection.h"
#include "bson_ruby_conversion.h"
#include "faststep_defines.h"
#include "utilities.h"
#include <string.h>

void faststep_collection_main() {
  rb_cFaststepCollection = rb_define_class_under(rb_mFaststep, "Collection", rb_cObject);

  rb_define_attr(rb_cFaststepCollection, "name", 1, 0);
  rb_define_attr(rb_cFaststepCollection, "db", 1, 0);

  rb_define_method(rb_cFaststepCollection, "initialize",   faststep_collection_init, 2);
  rb_define_method(rb_cFaststepCollection, "connection",   faststep_collection_connection, 0);
  rb_define_method(rb_cFaststepCollection, "ns",           faststep_collection_ns, 0);
  rb_define_method(rb_cFaststepCollection, "find",         faststep_collection_find, -1);
  rb_define_method(rb_cFaststepCollection, "find_one",     faststep_collection_find_one, -1);
  rb_define_method(rb_cFaststepCollection, "count",        faststep_collection_count, -1);
  rb_define_method(rb_cFaststepCollection, "insert",       faststep_collection_insert, -1);
  rb_define_method(rb_cFaststepCollection, "update",       faststep_collection_update, -1);
  rb_define_method(rb_cFaststepCollection, "remove",       faststep_collection_remove, -1);
  rb_define_method(rb_cFaststepCollection, "drop",         faststep_collection_drop, 0);
  rb_define_method(rb_cFaststepCollection, "create_index", faststep_collection_create_index, 1);
  return;
}

static VALUE faststep_collection_init(VALUE self, const VALUE name, const VALUE database) {
  rb_iv_set(self, "@name", name);
  rb_iv_set(self, "@db", database);

  return self;
}

static VALUE faststep_collection_connection(const VALUE self) {
  VALUE db = rb_iv_get(self, "@db");
  return rb_iv_get(db, "@connection");
}

VALUE faststep_collection_ns(const VALUE self) {
  VALUE db_name         = rb_str_new2(_ivar_name(rb_iv_get(self, "@db")));
  VALUE collection_name = rb_str_new2(_ivar_name(self));

  return build_collection_ns(db_name, collection_name);
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

static VALUE faststep_collection_find(int argc, VALUE* argv, VALUE self) {
  VALUE selector, options;
  rb_scan_args(argc, argv, "02", &selector, &options);
  if(!RTEST(selector)) { selector = rb_hash_new(); }
  if(!RTEST(options))  { options  = rb_hash_new(); }

  VALUE full_query = rb_hash_new();
  rb_hash_aset(full_query, rb_str_new2("selector"), selector);
  rb_funcall(full_query, rb_intern("merge"), 1, options);

  return rb_funcall(rb_cFaststepCursor, rb_intern("new"), 2, self, full_query);
}

static VALUE faststep_collection_find_one(int argc, VALUE* argv, VALUE self) {
  VALUE selector, options;
  rb_scan_args(argc, argv, "02", &selector, &options);

  if(!RTEST(options)) { options = rb_hash_new(); }

  VALUE full_query = rb_hash_new();

  if(RTEST(rb_funcall(selector, rb_intern("is_a?"), 1, rb_cBsonObjectId))) {
    rb_hash_aset(full_query, rb_str_new2("_id"), selector);
  } else {
    switch(TYPE(selector)) {
      case T_NIL:
        break;
      case T_HASH:
        full_query = selector;
        break;
      default:
        rb_raise(rb_eTypeError, "selector must be an instance of BSON::ObjectId, Hash, or nil");
    }
  }

  rb_hash_aset(options, rb_str_new2("limit"), INT2NUM(-1));

  VALUE result = rb_funcall(self, rb_intern("find"), 2, full_query, options);
  return rb_funcall(result, rb_intern("first"), 0);
}

VALUE build_collection_ns(const VALUE db_name, const VALUE collection_name) {
  VALUE ns = rb_str_new2("");

  rb_str_concat(ns, db_name);
  rb_str_concat(ns, rb_str_new2("."));
  rb_str_concat(ns, collection_name);

  return ns;
}

static VALUE faststep_collection_insert(int argc, VALUE* argv, const VALUE self) {
  mongo_connection* conn = GetFaststepConnectionForCollection(self);

  VALUE documents, options;
  rb_scan_args(argc, argv, "11", &documents, &options);

  VALUE ns = faststep_collection_ns(self);

  if(TYPE(documents) == T_ARRAY) {
    _faststep_collection_insert_batch(conn, RSTRING_PTR(ns), documents);
  } else {
    _faststep_collection_insert_one(conn, RSTRING_PTR(ns), documents);
  }

  return _faststep_safe_operation(self, options);
}

static VALUE faststep_collection_update(int argc, VALUE* argv, VALUE self) {
  VALUE query, operations, options;
  rb_scan_args(argc, argv, "21", &query, &operations, &options);

  bson* bson_query      = create_bson_from_ruby_hash(query);
  bson* bson_operations = create_bson_from_ruby_hash(operations);

  int update_flags = 0;

  if(TYPE(options) == T_HASH) {
    if(rb_indiff_hash_aref(options, rb_str_new2("multi")) == Qtrue) {
      update_flags |= MONGO_UPDATE_MULTI;
    }

    if(rb_indiff_hash_aref(options, rb_str_new2("upsert")) == Qtrue) {
      update_flags |= MONGO_UPDATE_UPSERT;
    }
  }

  mongo_update(GetFaststepConnectionForCollection(self),
               RSTRING_PTR(faststep_collection_ns(self)),
               bson_query,
               bson_operations,
               update_flags);

  bson_destroy(bson_query);
  bson_destroy(bson_operations);

  return _faststep_safe_operation(self, options);
}

static VALUE faststep_collection_remove(int argc, VALUE* argv, VALUE self) {
  VALUE query, options;
  rb_scan_args(argc, argv, "02", &query, &options);

  bson* bson_query = create_bson_from_ruby_hash(query);

  mongo_remove(GetFaststepConnectionForCollection(self),
               RSTRING_PTR(faststep_collection_ns(self)),
               bson_query);
  bson_destroy(bson_query);

  return _faststep_safe_operation(self, options);
}

static VALUE faststep_collection_drop(const VALUE self) {
  bson_bool_t result = mongo_cmd_drop_collection(GetFaststepConnectionForCollection(self),
                                                 _ivar_name(rb_iv_get(self, "@db")),
                                                 _ivar_name(self),
                                                 NULL);

  return bool_to_ruby(result);
}

static VALUE faststep_collection_create_index(const VALUE self, const VALUE indexes) {
  bson* bson_indexes = create_bson_from_ruby_hash(indexes);

  bson_bool_t result = mongo_create_index(GetFaststepConnectionForCollection(self),
                                          RSTRING_PTR(faststep_collection_ns(self)),
                                          bson_indexes,
                                          0,
                                          NULL);
  bson_destroy(bson_indexes);

  return bool_to_ruby(result);
}

static void _faststep_collection_insert_one(mongo_connection* conn, const char* ns, const VALUE document) {
  bson* bson_document = create_bson_from_ruby_hash(document);
  mongo_insert(conn, ns, bson_document);
  bson_destroy(bson_document);
}

static void _faststep_collection_insert_batch(mongo_connection* conn, const char* ns, const VALUE documents) {
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

static void _faststep_collection_destroy(bson** bson_documents, const int document_count) {
  int iterator;
  for(iterator = 0; iterator < document_count; iterator++) {
    bson_destroy(bson_documents[iterator]);
  }
  return;
}

static char* _ivar_name(const VALUE object) {
  return RSTRING_PTR(rb_iv_get(object, "@name"));
}

mongo_connection* GetFaststepConnectionForCollection(const VALUE collection) {
  VALUE db         = rb_iv_get(collection, "@db");
  VALUE connection = rb_iv_get(db, "@connection");

  return GetFaststepConnection(connection);
}

static VALUE _faststep_safe_operation(const VALUE self, const VALUE options) {
  int safe_mode = 0;

  if(TYPE(options) == T_HASH) {
    if(rb_indiff_hash_aref(options, rb_str_new2("safe")) == Qtrue) {
      safe_mode = 1;
    }
  }

  if(safe_mode) {
    VALUE document = rb_funcall(rb_iv_get(self, "@db"), rb_intern("get_last_error"), 0);
    ensure_document_ok(document);
    return document;
  } else {
    return Qtrue;
  }
}
