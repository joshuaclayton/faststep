module Faststep
  class Collection
    def index_information
      info = {}
      @db["system.indexes"].find({:ns => ns}).each do |index|
        info[index['name']] = index
      end
      info
    end
  end
end
