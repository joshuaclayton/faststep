require "spec_helper"

describe Faststep::Cursor do
  let(:connection) { Faststep::Connection.new("127.0.0.1", 27017) }
  let(:db)         { connection.db("faststep_test") }
  let(:collection) { db["something"] }

  before { db.drop }

  it "finds documents" do
    10.times { collection.insert(:foo => "bar") }

    collection.count.should == 10
    cursor = Faststep::Cursor.new(collection, {})
    cursor.map.length.should_not == 0
    cursor.each do |doc|
      doc[:foo].should == "bar"
    end
  end

  it "caches documents" do
    10.times { collection.insert(:foo => "bar") }

    cursor = Faststep::Cursor.new(collection, {})
    cursor.to_a.length.should == 10
    cursor.to_a.length.should == 10
  end
end
