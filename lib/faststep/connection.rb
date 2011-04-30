module Faststep
  class Connection
    DEFAULT_PORT = 27017

    attr_reader :host, :port

    def db(database_name)
      Faststep::Db.new(database_name, self)
    end

    alias [] db

    def drop_database(database_name)
      db(database_name).drop
    end
  end
end
