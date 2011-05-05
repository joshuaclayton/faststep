# -*- encoding: utf-8 -*-
$:.push File.expand_path("../lib", __FILE__)
require "faststep/version"

Gem::Specification.new do |s|
  s.name        = "faststep"
  s.version     = Faststep::VERSION
  s.platform    = Gem::Platform::RUBY
  s.authors     = ["Josh Clayton"]
  s.email       = ["joshua.clayton@gmail.com"]
  s.summary     = %q{Mongo on Speed}
  s.description = %q{Mongo on Speed}

  s.files         = `git ls-files`.split("\n")
  s.test_files    = `git ls-files -- {test,spec,features}/*`.split("\n")
  s.executables   = `git ls-files -- bin/*`.split("\n").map{ |f| File.basename(f) }
  s.require_paths = ["lib"]

  s.extensions = ["ext/faststep/extconf.rb"]

  s.add_dependency "rake-compiler", ">= 0.7.6"
  s.add_dependency "bson", ">= 1.2.4"
  s.add_dependency "bson_ext", ">= 1.2.4"

  s.add_development_dependency "rspec", ">= 2.5.0"
end
