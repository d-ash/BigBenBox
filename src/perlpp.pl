#
# PerlPP (PPP): Perl preprocessor
#
# http://darkness.codefu.org/wordpress/2003/03/perl-scoping/

package PerlPP;

use strict;
use warnings;
#use Data::Dumper;

my $TAG_OPEN = "<[?]";
my $TAG_CLOSE = "[?]>";

my $filename = "";
my $outFilename = "";
my $package = "";
my $code = "";
my $plain = "";
my $f;
my $PerlPP_out;

sub OutputPlain {
	$code .= "print \$PerlPP_out '" . $plain =~ s/'/\\'/gr . "';";
	$plain = "";
}

( $filename = shift ) and ( $outFilename = shift ) or die "Usage: perl perlpp.pl <pppFilename> <outFilename>\n";
$package = ( $filename =~ s/^([a-zA-Z_][a-zA-Z_0-9]*).ppp$/$1/r );

open $f, $filename or die $!;

OPENING:
while ( <$f> ) {
	if ( /^(.*?)$TAG_OPEN(.*)$/ ) {
		$plain .= $1;
		&OutputPlain;

		$_ = $2 . "\n";
		
		CLOSING:
		if ( /^(.*?)$TAG_CLOSE(.*)$/ ) {
			$code .= $1;
			$_ = $2 . "\n";
			redo OPENING;
		} else {
			$code .= $_ . "\n";
			$_ = <$f>;
			goto CLOSING;
		};
	} else {
		$plain .= $_;
	}
}
&OutputPlain;

close $f or die $!;

open $PerlPP_out, ">", "${outFilename}" or die $!;
eval( "package PPP_${package}; use strict; use warnings; sub echo { print \$PerlPP_out shift; }\n" . $code );
warn $@ if $@;
close $PerlPP_out or die $!;
