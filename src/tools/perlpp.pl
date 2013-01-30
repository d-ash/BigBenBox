#
# PerlPP (PPP): Perl preprocessor
#
# Usage: perl perlpp.pl [options] <inFilename> <outFilename>
#
# More info about scoping in Perl:
# http://darkness.codefu.org/wordpress/2003/03/perl-scoping/
#

package PerlPP;

use strict;
use warnings;

my $TAG_OPEN = "<[?]";
my $TAG_CLOSE = "[?]>";

my $argCommentsType = "";
my $argEval = "";
my $argDebug = 0;

my $filename = "";
my $outFilename = "";
my $package = "";
my $echoMode = 0;
my $code = "";
my $plain = "";
my $f;
my $PerlPP_out;

sub OutputPlain {
	$plain =~ s/\\/\\\\/g;
	$plain =~ s/'/\\'/g;
	$code .= "print \$PerlPP_out '${plain}';\n";
	$plain = "";
}

sub OutputComments {
	my $prefix = "";

	if ( $argCommentsType eq "doubleslash" ) {
		$prefix = "//";
	} elsif ( $argCommentsType eq "hash" ) {
		$prefix = "#";
	} else {
		print "Unknown type of comments style.\n";
		return;
	}

	print $PerlPP_out "${prefix} This file is automatically generated by perlpp.pl\n";
	print $PerlPP_out "${prefix}   source: $filename\n";
	print $PerlPP_out "${prefix}   package: $package\n\n";
}

while ( my $a = shift ) {
	if ( $a eq "--eval" ) {
		$argEval .= ( shift ) || "";
	} elsif ( $a eq "--debug" ) {
		$argDebug = 1;
	} elsif ( $a eq "--comments" ) {
		$argCommentsType = ( shift ) || "";
	} else {
		$filename = $a;
		( $outFilename = shift ) or die "Usage: perl perlpp.pl [options] <inFilename> <outFilename>\n";
	}
}

$package = $filename;
$package =~ s/^([a-zA-Z_][a-zA-Z_0-9.]*).p$/$1/;
$package =~ s/[.\/\\]/_/g;
$code = "package PPP_${package}; use strict; use warnings; sub echo { print \$PerlPP_out shift; }\n${argEval}\n";

open $f, $filename or die $!;

OPENING:
while ( <$f> ) {
	if ( /^(.*?)$TAG_OPEN(.*)$/ ) {
		my $after = $2;
		my $inside = "";

		$plain .= $1;
		&OutputPlain;

		if ( $after =~ /^=/ ) {
			$echoMode = 1;
			$_ = substr( $after, 1 ) . "\n";
		} else {
			$_ = $after . "\n";
		}
		
		CLOSING:
		if ( /^(.*?)$TAG_CLOSE(.*)$/ ) {
			$inside .= $1;
			if ( $echoMode ) {
				$code .= "print \$PerlPP_out ( ${inside} );\n";
				$echoMode = 0;
			} else {
				$code .= $inside;
			}
			$_ = $2 . "\n";
			redo OPENING;
		} else {
			$inside .= $_;
			$_ = <$f>;
			goto CLOSING;
		};
	} else {
		$plain .= $_;
	}
}
&OutputPlain;

close $f or die $!;

if ( $argDebug ) {
	print( $code );
} else {
	open $PerlPP_out, ">", "${outFilename}.tmp" or die $!;
	&OutputComments;
	eval( $code );
	warn $@ if $@;
	close $PerlPP_out or die $!;
    rename "${outFilename}.tmp", $outFilename;
}
