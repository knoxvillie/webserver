#!/usr/bin/env perl

print "HTTP/1.1 200 OK\r\n";
print "Content-Type: text/html\r\n";
print "Connection: keep-alive\r\n";
print "Content-length: 24\r\n";
print "\r\n";
print "Hello world! FROM PERL!!\r\n";
