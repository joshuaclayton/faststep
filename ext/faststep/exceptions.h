#ifndef EXCEPTIONS_H
#include <ruby.h>
#define EXCEPTIONS_H
void exceptions_main(VALUE);

#define RaiseFaststepException(exception_class, exception_message) {\
  VALUE faststep = rb_const_get(rb_cObject, rb_intern("Faststep"));\
  VALUE exception = rb_const_get(faststep, rb_intern(exception_class));\
  rb_raise(exception, exception_message);\
}

#endif
