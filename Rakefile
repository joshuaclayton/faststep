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

  faststep_bm_db.drop
  mongo_bm_db.connection.drop_database("mongo_bm")

  insert_document_small  = {:foo => "bar"}
  insert_document_medium = {"one" => "bar",
                            "two" => [1,2,3,4],
                            "three" => 5.times.map { BSON::ObjectId.new },
                            "nest" => {
                              "nest_again" => 2.times.map { BSON::ObjectId.new },
                              "more" => Time.now
                            }
                           }

  Benchmark.bm(50) do |benchmark|
    faststep_benchmark(benchmark, faststep_bm_db, :command => :insert, :times => 50000, :args => [insert_document_small])
    faststep_benchmark(benchmark, mongo_bm_db,    :command => :insert, :times => 50000, :args => [insert_document_small])

    faststep_benchmark(benchmark, faststep_bm_db, :command => :insert, :times => 50000, :args => [insert_document_medium])
    faststep_benchmark(benchmark, mongo_bm_db,    :command => :insert, :times => 50000, :args => [insert_document_medium])

    faststep_benchmark(benchmark, faststep_bm_db, :command => :count, :times => 10000)
    faststep_benchmark(benchmark, mongo_bm_db,    :command => :count, :times => 10000)
  end

  faststep_bm_db.drop
  Mongo::Connection.new.drop_database("mongo_bm")
end

def faststep_benchmark(benchmark, db_driver, options)
  command = options.delete(:command)
  times = options.delete(:times)
  benchmark.report("#{db_driver} #{command} #{times}x") do
    times.times do
      if !options[:args].nil?
        db_driver["stuff"].send(command, *options[:args])
      else
        db_driver["stuff"].send(command)
      end
    end
    db_driver["stuff"].count
  end
end
