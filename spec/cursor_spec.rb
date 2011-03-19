require "spec_helper"

describe Faststep::Cursor do
  let(:collection) { $faststep_test_db["something"] }

  it "finds documents" do
    10.times { collection.insert(:foo => "bar") }

    collection.count.should == 10
    cursor = Faststep::Cursor.new(collection, {})
    cursor.to_a.length.should_not == 0
    cursor.each do |doc|
      doc["foo"].should == "bar"
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

  it "limits documents" do
    10.times.map { |i| collection.insert(:name => "Person #{i}") }

    documents = collection.find({}, { "limit" => 2 }).to_a
    documents.map {|doc| doc["name"] }.should == ["Person 0", "Person 1"]

    documents = collection.find({}, { "limit" => -1 })
    documents.length.should == 1
    documents.first["name"].should == "Person 0"
  end

  it "skips documents" do
    collection.insert(:name => "John Doe",   :age => 25, :gender => "Male",   :postal_code => "02108")
    collection.insert(:name => "Jane Doe",   :age => 22, :gender => "Female", :postal_code => "02108")
    collection.insert(:name => "John Smith", :age => 40, :gender => "Male",   :postal_code => "02108")

    documents = collection.find({}, { "skip" => 1 }).to_a
    documents.length.should == 2
    documents[0]["name"].should == "Jane Doe"
    documents[1]["name"].should == "John Smith"
  end

  it "explains queries" do
    result = collection.find({}).explain
    result.should have_key("cursor")
    result.should have_key("nscanned")
    result.should have_key("n")
  end
end
