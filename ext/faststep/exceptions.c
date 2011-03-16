#include "exceptions.h"
#include "faststep_defines.h"

void faststep_exceptions_main() {
  rb_define_class_under(rb_mFaststep, "ConnectionFailure", rb_eStandardError);
  rb_define_class_under(rb_mFaststep, "OperationFailure", rb_eStandardError);
}

void RaiseFaststepException(char* exception_class_name, char* exception_message) {
  VALUE exception = rb_const_get(rb_mFaststep, rb_intern(exception_class_name));
  rb_raise(exception, exception_message);
  return;
}
