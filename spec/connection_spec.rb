require "spec_helper"

describe Faststep::Connection do
  subject { Faststep::Connection.new("127.0.0.1", 27017) }

  its(:host) { should == "127.0.0.1" }
  its(:port) { should == 27017 }
  it { should be_connected }
  it { should be_master }

  it "closes a connection to Mongo" do
    subject.disconnect!
    subject.should_not be_connected
    subject.connect!
    subject.should be_connected
  end
end

describe Faststep::Connection, "host and port defaults" do
  subject { Faststep::Connection.new }
  its(:host) { should == "127.0.0.1" }
  its(:port) { should == 27017 }
  it { should be_connected }
  it { should be_master }
end

describe Faststep::Connection, "getting a database" do
  subject { Faststep::Connection.new }

  it "uses a database" do
    subject[$faststep_test_db.name]["something"].insert(:foo => "bar")
    $faststep_test_db["something"].count.should == 1
  end
end

describe Faststep::Connection, "that doesn't connect due to misconfiguration" do
  it "raises an exception" do
    expect { Faststep::Connection.new("totally-bogus", 12345) }.to raise_error(Faststep::ConnectionFailure)
  end
end
