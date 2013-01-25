# Perl preprocessor

use strict;
use warnings;
use feature "state";

my $_TAG_OPEN = "<[?]";
my $_TAG_CLOSE = "[?]>";
my $_code = "";

sub Execute {
	eval( shift );
}

OPENING:
while ( <STDIN> ) {
	if ( /^(.*?)$_TAG_OPEN(.*)$/ ) {
		print $1;
		$_ = $2 . "\n";
		
		CLOSING:
		if ( /^(.*?)$_TAG_CLOSE(.*)$/ ) {
			Execute( $_code . $1 );
			warn $@ if $@;
			$_code = "";
			$_ = $2 . "\n";
			redo OPENING;
		} else {
			$_code .= $_ . "\n";
			$_ = <STDIN>;
			goto CLOSING;
		};
	} else {
		print $_;
	}
}
