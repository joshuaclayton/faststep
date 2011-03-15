#ifndef EXCEPTIONS_H
#include <ruby.h>
#define EXCEPTIONS_H
void exceptions_main();

#define RaiseFaststepException(exception_class, exception_message) {\
  VALUE exception = rb_const_get(rb_mFaststep, rb_intern(exception_class));\
  rb_raise(exception, exception_message);\
}

#endif
