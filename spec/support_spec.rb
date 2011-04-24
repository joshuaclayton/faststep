require "spec_helper"

describe Faststep::Support, "ok?" do
  it "returns true when no err or errmsg key is present" do
    Faststep::Support.ok?({ "ok" => true }).should be_true
    Faststep::Support.ok?({ "ok" => 1.0 }).should be_true
  end

  it "returns false when response is not ok" do
    Faststep::Support.ok?({ "err" => "something broke" }).should be_false
    Faststep::Support.ok?({ "errmsg" => "something else broke" }).should be_false
  end
end
