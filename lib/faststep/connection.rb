module Faststep
  class Connection
    DEFAULT_PORT = 27017

    def db(database_name)
      Db.new(database_name, self)
    end

    def drop_database(database_name)
      Db.new(database_name, self).drop
    end
  end
end
