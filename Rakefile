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

  insert_document_small  = {:foo => "bar"}
  insert_document_medium = {
    "one"   => "bar",
    "two"   => [1,2,3,4],
    "three" => 5.times.map { BSON::ObjectId.new },
    "nest"  => {
      "nest_again" => 2.times.map { BSON::ObjectId.new },
      "more" => Time.now
    }
  }

  insert_document_large = {
    "here" => 5.times.map { insert_document_small },
    "we"   => "ABCDEF"*5000,
    "go"   => 3.times.map { insert_document_medium }
  }


  faststep_bm_db = Faststep::Connection.new("127.0.0.1", 27017).db("faststep_bm")
  mongo_bm_db    = Mongo::Connection.new.db("mongo_bm")

  Benchmark.bmbm(50) do |benchmark|
    count = 30000

    faststep_benchmark(benchmark, faststep_bm_db, :command => :insert, :times => count, :args => insert_document_small)
    faststep_benchmark(benchmark, mongo_bm_db,    :command => :insert, :times => count, :args => insert_document_small)

    faststep_benchmark(benchmark, faststep_bm_db, :command => :insert, :times => count, :args => insert_document_medium)
    faststep_benchmark(benchmark, mongo_bm_db,    :command => :insert, :times => count, :args => insert_document_medium)

    faststep_benchmark(benchmark, faststep_bm_db, :command => :insert, :times => count/4, :args => insert_document_large)
    faststep_benchmark(benchmark, mongo_bm_db,    :command => :insert, :times => count/4, :args => insert_document_large)

    faststep_benchmark(benchmark, faststep_bm_db, :command => :count, :times => count/2)
    faststep_benchmark(benchmark, mongo_bm_db,    :command => :count, :times => count/2)
  end

  faststep_bm_db.drop
  Mongo::Connection.new.drop_database("mongo_bm")
end

def faststep_benchmark(benchmark, db_driver, options)
  command = options.delete(:command)
  times = options.delete(:times)

  benchmark.report("#{db_driver} #{command} #{times}x") do
    if !options[:args].nil?
      times.times do |i|
        # fucking object references that mongo modifies.
        doc = options[:args].inject({}) {|r,(k,v)| r[k]=v;r }
        db_driver["stuff"].send(command, doc)
      end
    else
      times.times do
        db_driver["stuff"].send(command)
      end
    end
    printf "%6d", db_driver["stuff"].count
  end
end
