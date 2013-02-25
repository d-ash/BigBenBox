#!/usr/bin/env perl

use v5.10;
use strict;
use warnings;

my @TESTS = qw(
	test_util
	test_util_hash
	test_bio
	test_sshot
);
my $arg = shift;

sub test_bio_Before {
	return;
}

sub test_bio_After {
	return;
}

if ( $arg && $arg eq "run" ) {
	foreach ( @TESTS ) {
		no strict "refs";			# is valid in a local scope only

		my $before = $_ . "_Before";
		my $after = $_ . "_After";

		say "--------------------------------------------- $_";
		if ( exists( &{ $before } ) ) {
			&{ $before };
		}
		system( "./$_" );
		if ( exists( &{ $after } ) ) {
			&{ $after };
		}
	}
} else {
	say join( "\n", @TESTS );
}
