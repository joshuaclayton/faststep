#include "support.h"
#include "faststep_defines.h"

void support_main() {
  rb_mFaststepSupport = rb_define_module_under(rb_mFaststep, "Support");
  rb_define_singleton_method(rb_mFaststepSupport, "ok?", support_ok, 1);
}

VALUE support_ok(VALUE self, VALUE document) {
  VALUE ok_value = rb_hash_aref(document, rb_str_new2("ok"));

  int result = 0;

  switch(TYPE(ok_value)) {
    case T_FLOAT: result = FIX2INT(ok_value); break;
    case T_TRUE:  result = 1; break;
  }

  return result ? Qtrue : Qfalse;
}
