#!/usr/bin/perl

use strict;
use DBI;


my $dbh = DBI->connect("DBI:Pg:dbname=void", "void", "tiTVPok?") or die "Database connect failed: $!";


print("Enter login name: ");

my $login = <STDIN>;

my $sql = "delete from ship where kowner in (select sname from player where  klogin = '$login');";

my $sth = $dbh->prepare($sql) or die "Could not delete $!";

$sth->execute() or die "$!";
$sth->finish();

$sql = "delete from player where klogin = '$login';";

my $sth = $dbh->prepare($sql) or die "Could not delete $!";
$sth->execute() or die "$!";
$sth->finish();



my $sql = "delete from login where slogin = '$login';";
my $sth = $dbh->prepare($sql) or die "Delete failed: $!";


$sth->execute() or die "$!";
$sth->finish();

$dbh->disconnect();
