require "spec_helper"

describe Faststep::Cursor do
  let(:collection) { $faststep_test_db["something"] }

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
