#!/usr/bin/perl

open(VOID_FIFO,">>/tmp/void-server") or die "Could not open FIFO";

print VOID_FIFO shift;
sleep(1);

