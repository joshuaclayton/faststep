#include "exceptions.h"
#include "faststep_defines.h"

void faststep_exceptions_main() {
  rb_eFaststepConnectionFailure = rb_define_class_under(rb_mFaststep, "ConnectionFailure", rb_eStandardError);
  rb_eFaststepOperationFailure  = rb_define_class_under(rb_mFaststep, "OperationFailure",  rb_eStandardError);
}
