module Faststep
  class Collection
    def find(selector = {}, options = {})
      Cursor.new(self, { "selector" => selector }.merge(options))
    end

    def find_one(spec_or_object_id=nil, opts={})
      spec = case spec_or_object_id
             when nil
               {}
             when BSON::ObjectId
               {:_id => spec_or_object_id}
             when Hash
               spec_or_object_id
             else
               raise TypeError, "spec_or_object_id must be an instance of ObjectId or Hash, or nil"
             end
      find(spec, opts.merge(:limit => -1)).first
    end
  end
end
