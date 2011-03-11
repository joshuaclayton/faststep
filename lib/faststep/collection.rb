module Faststep
  class Collection
    def find(selector)
      Cursor.new(self, selector)
    end
  end
end
