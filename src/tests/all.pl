use strict;
use warnings;

my $arg = shift;
my @TESTS = qw(
	test_util
	test_sshot
);

if ( $arg && $arg eq "run" ) {
	foreach ( @TESTS ) {
		print "::::: $_ :::::\n";
		system( "./$_.run" );
	}
} else {
	print join( " ", @TESTS ) . "\n";
}
