#include "faststep.h"
#include "connection.h"
#include "collection.h"

void Init_faststep() {
  VALUE Faststep = rb_define_module("Faststep");
  connection_main(Faststep);
  collection_main(Faststep);
}
