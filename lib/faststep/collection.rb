module Faststep
  class Collection
    def find(selector = {}, options = {})
      Cursor.new(self, "selector" => selector)
    end
  end
end
