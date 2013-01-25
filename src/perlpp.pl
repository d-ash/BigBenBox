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
my $f;
my $PerlPP_out;

sub Execute {
	eval( "package PPP_${package}; use strict; use warnings; " . shift );
}

( $filename = shift ) and ( $outFilename = shift ) or die "Usage: perl perlpp.pl <pppFilename> <outFilename>\n";
$package = ( $filename =~ s/^([a-zA-Z_][a-zA-Z_0-9]*).ppp$/$1/r );

open $f, $filename or die $!;
open $PerlPP_out, ">", "${outFilename}" or die $!;

Execute( "sub echo { print \$PerlPP_out shift; }" );

OPENING:
while ( <$f> ) {
	if ( /^(.*?)$TAG_OPEN(.*)$/ ) {
		print $PerlPP_out $1;
		$_ = $2 . "\n";
		
		CLOSING:
		if ( /^(.*?)$TAG_CLOSE(.*)$/ ) {
			Execute( $code . $1 );
			warn $@ if $@;
			$code = "";
			$_ = $2 . "\n";
			redo OPENING;
		} else {
			$code .= $_ . "\n";
			$_ = <$f>;
			goto CLOSING;
		};
	} else {
		print $PerlPP_out $_;
	}
}

close $f or die $!;
close $PerlPP_out or die $!;
