#include "faststep.h"
#include "connection.h"
#include "db.h"
#include "collection.h"
#include "exceptions.h"
#include "cursor.h"
#include "support.h"

VALUE rb_mBson;
VALUE rb_mFaststep;
VALUE rb_cFaststepConnection;
VALUE rb_cFaststepDb;
VALUE rb_cFaststepCollection;
VALUE rb_cFaststepCursor;
VALUE rb_mFaststepSupport;

void Init_faststep() {
  rb_mFaststep = rb_define_module("Faststep");
  rb_mBson     = rb_const_get(rb_cObject, rb_intern("BSON"));

  faststep_connection_main();
  faststep_db_main();
  faststep_collection_main();
  faststep_cursor_main();
  faststep_support_main();
  faststep_exceptions_main();
}
