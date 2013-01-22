use strict;
use warnings;

my @TESTS = qw(
	test_util
	test_util_bio
	test_sshot
);
my $arg = shift;

sub test_util_bio_Before {
	return;
}

sub test_util_bio_After {
	return;
}

if ( $arg && $arg eq "run" ) {
	foreach ( @TESTS ) {
		no strict "refs";	# is valid in a local scope only

		my $before = $_ . "_Before";
		my $after = $_ . "_After";

		print "--------------------------------------------- Running: $_\n";
		if ( exists( &{ $before } ) ) {
			&{ $before };
		}
		system( "./$_.run" );
		if ( exists( &{ $after } ) ) {
			&{ $after };
		}
	}
} else {
	print join( " ", @TESTS ) . "\n";
}