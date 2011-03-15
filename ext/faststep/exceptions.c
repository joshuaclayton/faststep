#include "exceptions.h"
#include "faststep_defines.h"

void exceptions_main() {
  rb_define_class_under(rb_mFaststep, "ConnectionFailure", rb_eStandardError);
  rb_define_class_under(rb_mFaststep, "OperationFailure", rb_eStandardError);
}
