#!/usr/bin/ruby

require "bartlby/bartlby"

cfg1 ="/storage/SF.NET/BARTLBY/GIT/bartlby-ruby/../bartlby-core/BARTLBY.local"

print "####Bartlby init instance #{cfg1}\n"
a = Bartlby.new cfg1


cfg2 ="/storage/SF.NET/BARTLBY/GIT/bartlby-ruby/../bartlby-core/BARTLBY.local1";
print "####Bartlby init instance #{cfg2}\n"
b = Bartlby.new cfg2


print "INSPECT INSTANCE1";
print a.inspect;
print "\n";
print a.info.inspect
print "\n";
print b.lib_info.inspect
print "\n";
print "INSPECT INSTANCE2";
print b.inspect;
print "\n";
print a.info.inspect
print "\n";
print b.lib_info.inspect
print "\n";


a.close
b.close

#print a.inspect
