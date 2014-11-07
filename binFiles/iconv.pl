#!/usr/bin/perl -w

use strict;
use Encode;
use Getopt::Std ;

my ($usage) = <<EOT

  [perl] iconv.pl -f <original-encoding> -t <target-encoding> < <file>

EOT
;

our ($opt_f, $opt_t);
getopts("f:t:") ;

if ((!defined $opt_f) || (!defined $opt_t))
{
  die $usage;
}

while(<STDIN>) {
        chomp($_);
        Encode::from_to($_, $opt_f, $opt_t);
        print  $_, "\n";
}

