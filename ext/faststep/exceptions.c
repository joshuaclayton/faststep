#include "exceptions.h"

void exceptions_main(VALUE faststep) {
  rb_define_class_under(faststep, "ConnectionFailure", rb_eStandardError);
}
