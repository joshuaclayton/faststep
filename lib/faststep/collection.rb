module Faststep
  class Collection
    def index_information
      info = {}
      db["system.indexes"].find({:ns => ns}).each do |index|
        info[index['name']] = index
      end
      info
    end

    def rename(new_name)
      connection["admin"].command(:renameCollection => ns, :to => "#{db.name}.#{new_name}")
      @name = new_name
      true
    end
  end
end
