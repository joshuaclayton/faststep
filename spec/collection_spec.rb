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

  it "finds single documents" do
    10.times { db["something"].insert(:foo => "bar") }
    db["something"].find_one({:foo => "bar"})["foo"].should == "bar"
    db["something"].find_one(BSON::ObjectId.new).should be_nil
  end

  it "updates documents" do
    db["something"].insert(:foo => "bar", :something => "fun")
    db["something"].update({ :something => "fun" }, { "$set" => { :foo => "awesome" } })

    db["something"].find({}).first["foo"].should == "awesome"
  end

  it "removes documents" do
    db["something"].insert(5.times.map { {:foo => "bar", :something => "fun"} })
    db["something"].insert(:foo => "bar", :baz => "what!")

    db["something"].remove(:baz => "what!")
    db["something"].count.should == 5
    db["something"].remove
    db["something"].count.should be_zero
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

describe Faststep::Collection, "#insert" do
  let(:large) {
    {
      'base_url' => 'http://www.example.com/test-me',
      'total_word_count' => 6743,
      'access_time' => Time.now,
      'meta_tags' => {
        'description' => 'i am a long description string',
        'author' => 'Holly Man',
        'dynamically_created_meta_tag' => 'who know\n what'
      },
      'page_structure' => {
        'counted_tags' => 3450,
        'no_of_js_attached' => 10,
        'no_of_images' => 6
      },
      'harvested_words' => ['10gen','web','open','source','application','paas',
        'platform-as-a-service','technology','helps',
        'developers','focus','building','mongodb','mongo'] * 20
    }
  }

  let(:db) { $faststep_test_db }
  let(:document_count) { 50 }

  it "batch inserts multiple large documents" do
    db["something"].insert([large] * document_count)
    db["something"].count.should == document_count
    db["something"].count("base_url" => large["base_url"]).should == document_count
  end
end
