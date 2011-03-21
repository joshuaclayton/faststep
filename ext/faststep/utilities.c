#include "utilities.h"

VALUE rb_str_to_sym(const VALUE str) {
  return ID2SYM(rb_intern(RSTRING_PTR(str)));
}

VALUE rb_indiff_hash_aref(const VALUE hash, const VALUE key) {
  VALUE string_key, symbol_key;

  if(RTEST(string_key = rb_hash_aref(hash, key))) {
    return string_key;
  } else if(RTEST(symbol_key = rb_hash_aref(hash, rb_str_to_sym(key)))) {
    return symbol_key;
  } else {
    return Qnil;
  }
}
