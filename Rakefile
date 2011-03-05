require "bundler"
Bundler::GemHelper.install_tasks

require "rake/extensiontask"
Rake::ExtensionTask.new("faststep") do |extension|
  extension.lib_dir = "lib/faststep"
end

task :build => [:clean, :compile]

require "spec/rake/spectask"

Spec::Rake::SpecTask.new(:spec => :build) do |t|
  t.spec_files = FileList['spec/**/*.rb']
end

task :default => :spec
