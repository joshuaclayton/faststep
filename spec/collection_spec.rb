require "spec_helper"

describe Faststep::Collection do
  let(:db) { $faststep_test_db }

  it "inserts the correct documents" do
    db["something"].insert(:foo => "bar")
    db["another.thing"].insert(:baz => "qux")

    db["something"].count(:foo => "bar").should == 1
    db["something"].count(:baz => "qux").should == 0
    db["another.thing"].count(:foo => "bar").should == 0
    db["another.thing"].count(:foo => true).should == 0
    db["another.thing"].count(:baz => "qux").should == 1
  end

  it "supports batch inserting" do
    db["something"].insert([{:foo => "bar"}, {:baz => "qux"}])

    db["something"].count.should == 2
    db["something"].count(:foo => "bar").should == 1
  end

  it "finds documents" do
    db["something"].insert(:foo => "bar")
    db["another.thing"].insert(:baz => "qux")

    db["something"].find({}).first["foo"].should == "bar"
    db["another.thing"].find({}).first["baz"].should == "qux"
  end

  it "updates documents" do
    db["something"].insert(:foo => "bar", :something => "fun")
    db["something"].update({ :something => "fun" }, { "$set" => { :foo => "awesome" } })

    db["something"].find({}).first["foo"].should == "awesome"
  end

  it "drops collections" do
    db["something"].insert(:foo => "bar")
    db["something"].drop
    db["something"].count.should be_zero
  end
end

describe Faststep::Collection, "queries" do
  let(:db) { $faststep_test_db }

  before do
    db["something"].insert(:foo => "bar")
    db["something"].insert(:baz => "qux")
    db["another.thing"].insert(:baz => "qux")
  end

  it "handles equality" do
    db["something"].find(:foo => "bar").to_a.tap do |documents|
      documents.length.should == 1
      documents.first["foo"].should == "bar"
    end
  end

  it "handles $in" do
    db["something"].find(:foo => {"$in" => [nil, "bar"]}).to_a.tap do |documents|
      documents.length.should == 2
      documents.first["foo"].should == "bar"
    end
  end

  it "handles regexes" do
    db["something"].find(:baz => /q.[xyz]/).to_a.tap do |documents|
      documents.length.should == 1
      documents.first["baz"].should == "qux"
    end
  end
end
