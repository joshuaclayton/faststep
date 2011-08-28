require "rspec"
require "faststep"

$faststep_test_db = Faststep::Connection.new("127.0.0.1", 27017).db("faststep_test")

RSpec.configure do |config|
  config.before do
    $faststep_test_db.collections.each do |collection|
      next if collection.name == 'system.indexes'
      collection.drop
    end
  end
end
