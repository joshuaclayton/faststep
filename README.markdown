# Faststep

## Mongo on Speed

### Install

Add to your Gemfile and `bundle`; otherwise,

    gem install faststep

### Usage

Connect to mongo:

    >> conn = Faststep::Connection.new("127.0.0.1", 27017)

Pick a database:

    >> db = conn.db("awesome_development")

Have some fun!

    >> db["users"].count
    => 0
    >> db["users"].insert(10000.times.map {|i| { :name => "Person #{i}" } }
    => true
    >> db["users"].count
    => 10000
    >> db["users"].count(:name => /0$/)
    => 1000
    >> db["users"].find(:name => /0$/).to_a
    => [{"_id"=>BSON::ObjectId('4d83c53555ca381d572c1cf7'), "name"=>"Person 0"}, {"_id"=>BSON::ObjectId('4d83c53555ca381d572c1d01'), "name"=>"Person 10"}, ...]
    >> db["users"].remove(:name => /0$/)
    => nil
    >> db["users"].count(:name => /0$/)
    => 0
    >> db["users"].count
    => 9000
    >> db["users"].drop
    => true
    >> db.drop
    => true

### Caveats

This is under heavy development (what gem isn't?) but there's a lot of stuff to be implemented still. Some methods return nil (remove, for example) when they shouldn't.
It's not nearly as smart as the mongo Ruby driver and it doesn't handle errors that well.

### Todo

* Better querying
  * Limits/hint, etc.
* Connection pooling
* Safe mode
* Allow updates to *not* be multi (currently unconfigurable)
* Master/slave
* Replica sets

Basically, most of http://www.mongodb.org/display/DOCS/Feature+Checklist+for+Mongo+Drivers

### About

This is a C extension that uses the [Mongo C driver](https://github.com/mongodb/mongo-c-driver).
As of right now, I haven't modified any of the code provided in that driver in here.
That code's Apache license still applies, and since it's not a derivative work (I haven't
changed any of the source), I'm using an MIT license for Faststep.

I've basically copied the benchmark from the [Mongo Ruby driver](https://github.com/mongodb/mongo-ruby-driver)
to test Faststep's speed, and all operations are anywhere between 10% (finding ranges on large docs)
and 120% faster (inserting small/medium indexed documents and inserting small/medium documents in batches).
See https://gist.github.com/870297 for benchmarks.

I started writing this driver for a project I'm working on called [Crowdtap](http://crowdtap.com),
which uses Mongo as its primary data store. I'd dabbled in C extensions before and
figured I could leverage this knowledge to speed up certain operations we were doing.
If it wasn't for them and their belief that I could get something usable in a short time,
this project probably wouldn't exist; thanks guys!

### License

The Mongo C driver code has retained its Apache license; I've released faststep under an MIT license (see
LICENSE for details).

### Author

Written 2011 by Josh Clayton.
