require "spec_helper"

describe Faststep::Db do
  let(:connection) { Faststep::Connection.new("127.0.0.1", 27017) }
  subject          { connection.db("faststep_test") }

  it "knows its collection names" do
    subject["something"].insert(:foo => "bar")
    subject["another.thing"].insert(:baz => "qux")

    subject.collection_names.should include("something", "another.thing")
    subject.collection_names.should_not include(/$/)
  end

  it "drops the database" do
    2.times { subject["something"].insert(:foo => "bar") }
    2.times { subject["another.thing"].insert(:baz => "qux") }

    subject.drop

    subject["something"].count.should == 0
    subject["another.thing"].count.should == 0
  end
end
