#include <ruby.h>
#include <math.h>
#include "faststep.h"

void Init_faststep() {
  Faststep           = rb_define_module("Faststep");
  FaststepConnection = rb_define_class_under(Faststep, "Connection", rb_cObject);
}
