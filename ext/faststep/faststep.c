#include "faststep.h"
#include "connection.h"
#include "db.h"
#include "collection.h"
#include "exceptions.h"
#include "cursor.h"

void Init_faststep() {
  VALUE Faststep = rb_define_module("Faststep");
  connection_main(Faststep);
  db_main(Faststep);
  collection_main(Faststep);
  exceptions_main(Faststep);
  cursor_main(Faststep);
}
