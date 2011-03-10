module Faststep
  class Connection
    def db(database_name)
      Db.new(database_name, self)
    end
  end
end
