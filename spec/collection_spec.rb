require "spec_helper"

describe Faststep::Collection do
  let(:db) { $faststep_test_db }

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
  let(:small_document) {
    { "foo" => "bar" }
  }

  let(:large_document) {
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

  it "inserts the correct documents" do
    db["something"].insert(:foo => "bar")
    db["another.thing"].insert(:baz => "qux")

    db["something"].count(:foo => "bar").should == 1
    db["something"].count(:baz => "qux").should == 0
    db["another.thing"].count(:foo => "bar").should == 0
    db["another.thing"].count(:foo => true).should == 0
    db["another.thing"].count(:baz => "qux").should == 1
  end

  it "inserts documents in safe mode" do
    document_id = BSON::ObjectId.new
    db["something"].insert({ :_id => document_id, :something => "great" }, { :safe => true })

    expect {
      db["something"].insert({ :_id => document_id, :something => "great" })
    }.to_not raise_error

    expect {
      db["something"].insert({ :_id => document_id, :something => "great" },
                             { :safe => true })
    }.to raise_error(Faststep::OperationFailure, /Error \(11000\): .*duplicate key error index/)
  end

  it "supports batch inserting" do
    db["something"].insert([small_document] * document_count)
    db["something"].count("foo" => "bar").should == document_count
  end

  it "batch inserts multiple large documents" do
    db["something"].insert([large_document] * document_count)
    db["something"].count.should == document_count
    db["something"].count("base_url" => large_document["base_url"]).should == document_count
  end
end

describe Faststep::Collection, "#update" do
  let(:db) { $faststep_test_db }

  it "updates documents" do
    2.times { db["something"].insert(:foo => "bar", :something => "fun") }

    db["something"].update({ :something => "fun" }, { "$set" => { :foo => "awesome" } })
    db["something"].count(:foo => "awesome").should == 1
    db["something"].count(:foo => "bar").should == 1
  end

  it "multi-updates documents" do
    2.times { db["something"].insert(:foo => "bar", :something => "fun") }

    db["something"].update({ :something => "fun" }, { "$set" => { :foo => "awesome" } }, :multi => true)
    db["something"].count(:foo => "awesome").should == 2
    db["something"].count(:foo => "bar").should be_zero
  end

  it "upserts documents" do
    2.times do
      db["something"].update({ "email" => "john@example.com" }, { "$inc" => { "count" => 1 } }, :upsert => true)
    end

    db["something"].count.should == 1
    db["something"].find_one["count"].should == 2
  end

  it "updates documents in safe mode" do
    expect {
      db["something"].update({},
                             { "$set" => { "invalid." => 1 } },
                             { :safe => true })
    }.to raise_error(Faststep::OperationFailure, /Error \(10149\): Invalid mod field name/)
  end
end

describe Faststep::Collection, "#remove" do
  let(:db) { $faststep_test_db }

  before do
    db["something"].insert(5.times.map { {:foo => "bar", :something => "fun"} })
    db["something"].insert(:foo => "bar", :baz => "what!")

  end

  it "removes documents" do
    db["something"].remove(:baz => "what!")
    db["something"].count.should == 5
    db["something"].remove
    db["something"].count.should be_zero
  end

  it "removes documents in safe mode" do
    db["something"].remove({}, { :safe => true })["n"].should == 6
  end
end

describe Faststep::Collection, "indexes" do
  let(:collection) { $faststep_test_db["something"] }

  it "creates indexes" do
    collection.create_index(:foo => 1)
    collection.index_information.should == {
      "_id_" => { "name" => "_id_", "ns" => collection.ns, "key" => { "_id" => 1 }, "v" => 0 },
      "_foo" => { "name" => "_foo", "ns" => collection.ns, "key" => { "foo" => 1 }, "v" => 0 }
    }
  end
end

describe Faststep::Collection, "#rename" do
  let(:collection) { $faststep_test_db["something"] }

  it "renames collections" do
    collection.insert(:foo => "bar")
    collection.rename("something_else")
    collection.name.should == "something_else"
    $faststep_test_db["something_else"].count.should == 1
  end
end
