require "spec_helper"

describe Faststep::Connection do
  subject { Faststep::Connection.new("127.0.0.1", 27017) }

  it "knows its host" do
    subject.host == "127.0.0.1"
  end

  it "knows its port" do
    subject.port == 27017
  end

  it "connects to Mongo automatically" do
    subject.should be_connected
  end

  it "closes a connection to Mongo" do
    subject.disconnect!
    subject.should_not be_connected
    subject.connect!
    subject.should be_connected
  end
end
