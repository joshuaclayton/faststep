require "spec_helper"

describe Faststep::Collection do
  let(:connection) { Faststep::Connection.new("127.0.0.1", 27017) }
  let(:db)         { connection.db("faststep_test") }

  before { db.drop }

  it "inserts the correct documents" do
    db["something"].insert(:foo => "bar")
    db["another.thing"].insert(:baz => "qux")

    db["something"].count(:foo => "bar").should == 1
    db["something"].count(:baz => "qux").should == 0
    db["another.thing"].count(:foo => "bar").should == 0
    db["another.thing"].count(:foo => true).should == 0
    db["another.thing"].count(:baz => "qux").should == 1
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
end
