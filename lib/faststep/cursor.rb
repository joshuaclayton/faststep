module Faststep
  class Cursor
    include Enumerable

    attr_reader :collection

    def skip(count)
      @skip = count
      self
    end

    def limit(count)
      @limit = count
      self
    end

    def fields(field_array)
      @fields = field_array
      self
    end

    def to_a
      super
    end
  end
end
