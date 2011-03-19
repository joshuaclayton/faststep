require "spec_helper"

describe Faststep::Cursor do
  let(:collection) { $faststep_test_db["something"] }

  it "finds documents" do
    10.times { collection.insert(:foo => "bar") }

    collection.count.should == 10
    cursor = Faststep::Cursor.new(collection, {})
    cursor.to_a.length.should_not == 0
    cursor.each do |doc|
      doc[:foo].should == "bar"
    end
  end

  it "finds specific fields for a document" do
    collection.insert(:name => "John Doe", :age => 25, :gender => "Male",   :postal_code => "02108")
    collection.insert(:name => "Jane Doe", :age => 22, :gender => "Female", :postal_code => "02108")

    documents = collection.find({}, { "fields" => %w(name age) }).to_a

    documents.each do |document|
      document.has_key?("name").should be_true
      document.has_key?("age").should be_true
      document.has_key?("_id").should be_true
      document.has_key?("gender").should be_false
      document.has_key?("postal_code").should be_false
    end
  end

  it "caches documents" do
    10.times { collection.insert(:foo => "bar") }

    cursor = Faststep::Cursor.new(collection, {})
    cursor.to_a.length.should == 10
    cursor.to_a.length.should == 10
  end
end
