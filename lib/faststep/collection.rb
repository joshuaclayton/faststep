module Faststep
  class Collection
    attr_reader :name, :db

    def initialize(name, db)
      @name = name
      @db   = db
    end

    def connection
      db.connection
    end

    def index_information
      info = {}
      db["system.indexes"].find({:ns => ns}).each do |index|
        info[index['name']] = index
      end
      info
    end

    def rename(new_name)
      command = BSON::OrderedHash.new
      command[:renameCollection] = ns
      command[:to] = "#{db.name}.#{new_name}"
      connection["admin"].command(command)
      @name = new_name
      true
    end

    def drop_index(index_name)
      db.command(:deleteIndexes => self.name, :index => index_name)
      true
    end
  end
end
