require "spec_helper"

describe Faststep::Support, "ok?" do
  it "returns true when response is ok" do
    Faststep::Support.ok?({ "ok" => true }).should be_true
    Faststep::Support.ok?({ "ok" => 1.0 }).should be_true
  end

  it "returns false when response is not ok" do
    Faststep::Support.ok?({ "ok" => false }).should be_false
    Faststep::Support.ok?({ "ok" => 0.0 }).should be_false
    Faststep::Support.ok?({ "ok" => nil }).should be_false
  end
end
