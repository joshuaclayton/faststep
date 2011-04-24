#include "support.h"
#include "faststep_defines.h"
#include "bson_ruby_conversion.h"

void faststep_support_main() {
  rb_mFaststepSupport = rb_define_module_under(rb_mFaststep, "Support");
  rb_define_singleton_method(rb_mFaststepSupport, "ok?", faststep_support_ok, 1);
}

static VALUE faststep_support_ok(VALUE self, VALUE document) {
  bson_bool_t result = 1;
  if(RTEST(rb_hash_aref(document, rb_str_new2("err"))) ||
      RTEST(rb_hash_aref(document, rb_str_new2("errmsg")))) {
    result = 0;
  }

  return bool_to_ruby(result);
}
