require "bundler"
Bundler::GemHelper.install_tasks

require "rake/extensiontask"
Rake::ExtensionTask.new("faststep") do |extension|
  extension.lib_dir = "lib/faststep"
end

task :build => [:clean, :compile]

require "rspec/core/rake_task"

RSpec::Core::RakeTask.new(:spec => :build)

task :default => :spec

desc "Benchmark C implementation against pure Ruby implementation of the Mongo Ruby driver"
task(:benchmark => :build) do
  require "benchmark"
  $LOAD_PATH.unshift(File.join(File.dirname(__FILE__), "lib"))
  require "faststep"
  require "mongo"
  require "bson"

  faststep_bm_db = Faststep::Connection.new("127.0.0.1", 27017).db("faststep_bm")
  mongo_bm_db    = Mongo::Connection.new.db("mongo_bm")

  insert_document_small  = {:foo => "bar"}
  insert_document_medium = {"one" => "bar",
                            "two" => [1,2,3,4],
                            "three" => 5.times.map { BSON::ObjectId.new },
                            "nest" => {
                              "nest_again" => 2.times.map { BSON::ObjectId.new },
                              "more" => Time.now
                            }
                           }

  Benchmark.bm(30) do |benchmark|
    times = 5000
    benchmark.report("Mongo count #{times}x") do
      times.times do
        mongo_bm_db["stuff"].count
      end
    end

    benchmark.report("Faststep count #{times}x") do
      times.times do |i|
        faststep_bm_db["stuff"].count
      end
    end
  end

  faststep_bm_db.drop
  Mongo::Connection.new.drop_database("mongo_bm")
end
