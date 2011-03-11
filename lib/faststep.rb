require "bson"

module Faststep
  autoload :Db,         "faststep/db"
  autoload :Connection, "faststep/connection"
  autoload :Collection, "faststep/collection"
  autoload :Cursor,     "faststep/cursor"
end

require "faststep/faststep"
