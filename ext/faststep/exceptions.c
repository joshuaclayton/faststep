#include "exceptions.h"

void exceptions_main(VALUE faststep) {
  rb_define_class_under(faststep, "ConnectionFailure", rb_eStandardError);
  rb_define_class_under(faststep, "OperationFailure", rb_eStandardError);
}
