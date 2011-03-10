module Faststep
  class Db
    attr_reader :name, :connection

    def initialize(name, connection)
      @name = name
      @connection = connection
    end

    def collection(collection_name)
      Collection.new(collection_name, self)
    end

    alias [] collection
  end
end
