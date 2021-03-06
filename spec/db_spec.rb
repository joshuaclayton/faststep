require "spec_helper"

describe Faststep::Db do
  subject { $faststep_test_db }

  it "knows its collection names" do
    subject["something"].insert(:foo => "bar")
    subject["another.thing"].insert(:baz => "qux")

    subject.collection_names.should include("something", "another.thing")
    subject.collection_names.should_not include(/$/)
  end

  it "runs specific commands" do
    expect { subject.command(:dbstats => 1) }.to_not raise_error
    expect {
      subject.command(:totally_bogus => 1)
    }.to raise_error(Faststep::OperationFailure,
                     %{Invalid command ({"totally_bogus"=>1}): no such cmd: totally_bogus})
  end

  it "drops the database" do
    2.times { subject["something"].insert(:foo => "bar") }
    2.times { subject["another.thing"].insert(:baz => "qux") }

    subject.drop

    subject["something"].count.should be_zero
    subject["another.thing"].count.should be_zero
  end

  it "gets the last error" do
    subject["something"].insert(:foo => "bar")
    subject.get_last_error.tap do |result|
      result.should have_key("ok")
      result.should have_key("n")
      result.should have_key("err")
    end
  end
end
