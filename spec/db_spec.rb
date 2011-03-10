require "spec_helper"

describe Faststep::Db do
  let(:connection) { Faststep::Connection.new("127.0.0.1", 27017) }
  subject          { connection.db("faststep-test") }

  it "knows its collection names" do
    subject["something"].insert(:foo => "bar")
    subject["another.thing"].insert(:baz => "qux")

    subject.collection_names.should include("something", "another.thing")
    subject.collection_names.should_not include(/$/)
  end
end
