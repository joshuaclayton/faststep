module Faststep
  class Db
    def collection(collection_name)
      Collection.new(collection_name, self)
    end
  end
end
