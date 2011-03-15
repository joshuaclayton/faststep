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
