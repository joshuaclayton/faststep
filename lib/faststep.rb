require "bson"

module Faststep
  ASCENDING  =  1
  DESCENDING = -1
  autoload :Db,         "faststep/db"
  autoload :Connection, "faststep/connection"
  autoload :Cursor,     "faststep/cursor"
end

require "faststep/faststep"
