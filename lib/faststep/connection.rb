module Faststep
  class Connection
    def drop_database(database_name)
      db(database_name).drop
    end
  end
end
