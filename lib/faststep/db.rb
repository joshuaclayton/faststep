module Faststep
  class Db
    attr_reader :name, :connection

    def initialize(name, connection)
      @name       = name
      @connection = connection
    end

    def collection(collection_name)
      Collection.new(collection_name, self)
    end

    alias [] collection

    def collection_names
      names = collections_info.map {|doc| doc["name"] }
      names.reject! {|name| name.index(@name).nil? || name.index('$') }
      names.map {|name| name.sub("#{@name}.", "") }
    end

    def collections
      collection_names.map do |collection_name|
        Collection.new(collection_name, self)
      end
    end

    private

    def collections_info
      Cursor.new(Collection.new("system.namespaces", self), :selector => {}).to_a
    end
  end
end
