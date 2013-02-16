#!/usr/bin/env perl

=pod
	PerlPP (PPP): Perl preprocessor
	by d-ash <andrey.shubin@gmail.com>

	Usage: ./perlpp.pl [options] filename

	Some info about scoping in Perl:
	http://darkness.codefu.org/wordpress/2003/03/perl-scoping/
=cut

package PerlPP;

use strict;
use warnings;

use constant TAG_OPEN => "<?";
use constant TAG_CLOSE => "?>";
use constant OPENING_RE => qr/^(.*?)\Q${\(TAG_OPEN)}\E(.*)$/;
use constant CLOSING_RE => qr/^(.*?)\Q${\(TAG_CLOSE)}\E(.*)$/;

my $package = "";

my $echoMode = 0;
my $commandMode = 0;
my $command = "";
my $catching = 0;
my $wasCatched = 0;
my $code = "";

my $RootSTDOUT;
my @OutputBuffers = ();
my $CGuard = "";
my %Prefixes = ();
my %CCleanups = ();

sub StartOB {
	if ( scalar @OutputBuffers == 0 ) {
		$| = 1;					# flush a contents of STDOUT
		open( $RootSTDOUT, ">&STDOUT" ) or die $!;		# dup filehandle
	}
	push( @OutputBuffers, "" );
	close( STDOUT );			# must be closed before redirecting it to a variable
	open( STDOUT, ">>", \$OutputBuffers[ $#OutputBuffers ] ) or die $!;
	$| = 1;						# do not use output buffering
}

sub EndOB {
	my $s;

	$s = pop( @OutputBuffers );
	close STDOUT;
	if ( scalar @OutputBuffers == 0 ) {
		open( STDOUT, ">&", $RootSTDOUT ) or die $!;	# dup filehandle
		$| = 0;					# return output buffering to the default state
	} else {
		open( STDOUT, ">>", \$OutputBuffers[ $#OutputBuffers ] ) or die $!;
	}
	return $s;
}

sub ReadOB {
	my $s;

	$s = $OutputBuffers[ $#OutputBuffers ];
	$OutputBuffers[ $#OutputBuffers ] = "";
	return $s;
}

=pod
	print "start " .
		do { StartOB(); print "X" .
			do { StartOB(); print "very long text"; StartOB(); print "internal"; print EndOB(); print "much more"; EndOB(); } .
		"Z"; EndOB(); } .
	" end\n";
=cut

sub OutputPlain {
	my $_;
	my $plain;

	$plain = ReadOB();
	foreach ( keys %Prefixes ) {
		$plain =~ s/(^|\W)\Q$_\E/$1$Prefixes{ $_ }/g;
	}
	$plain =~ s/\\/\\\\/g;
	$plain =~ s/'/\\'/g;

	if ( $catching ) {
		if ( $commandMode ) {
			$command .= "'${plain}'";
		} else {
			print "'${plain}'";
		}
	} else {
		$code .= "print '${plain}';\n";
	}
}

sub ProcessCommand {
	my $cmd = shift;

	if ( $cmd =~ /^prefix\s+(\S+)\s+(\S+)\s*$/i ) {
		$Prefixes{ $1 } = $2;
	} elsif ( $cmd =~ /^c:guard\s*$/i ) {
		$CGuard = "_PPP_CGUARD_" . uc( $package );
		print "#ifndef ${CGuard}\n#define ${CGuard}\n"
	} elsif ( $cmd =~ /^c:onCleanup\s+([a-zA-Z_][a-zA-Z_0-9]*)\s+(.*)$/is ) {
		#print STDERR "c:onCleanup $1 $2\n";
	} elsif ( $cmd =~ /^c:gotoCleanup\s*$/i ) {
		#print STDERR "c:gotoCleanup\n";
	} elsif ( $cmd =~ /^c:cleanup\s*$/i ) {
		#print STDERR "c:cleanup\n";
	} else {
		die "Unknown command: ${cmd}";
	}
}

sub ParseFile {
	my $fname = shift;
	my $f;
	
	open( $f, "<", $fname ) or die $!;
	StartOB();

OPENING:
	while ( <$f> ) {
		# 'redo OPENING' jumps in here
		if ( $_ =~ OPENING_RE ) {
			my $after = $2;
			my $inside = "";

			print $1;
			OutputPlain();

			$wasCatched = 0;
			if ( $catching ) {
				if ( $after =~ /^"/ ) {								# end of catching
					$catching = 0;
					$code .= EndOB();
					$wasCatched = 1;
					$_ = substr( $after, 1 ) . "\n";
				} else {											# code execution within catching
					die "Unfinished catching.";
				}
			} else {
				if ( $after =~ /^=/ ) {
					$echoMode = 1;
					$_ = substr( $after, 1 ) . "\n";
				} elsif ( $after =~ /^:/ ) {
					$commandMode = 1;
					$_ = substr( $after, 1 ) . "\n";
				} elsif ( $after =~ /^"/ ) {
					die "Unexpected end of catching, it was not started.";
				} else {
					$_ = $after . "\n";
				}
			}

CLOSING:
			if ( $_ =~ CLOSING_RE ) {
				$inside .= $1;
				$_ = $2 . "\n";				# it will be processed after 'redo OPENING'
				if ( $inside =~ /"$/ ) {
					$inside = substr( $inside, 0, -1 );
					if ( $echoMode ) {
						# echoMode is transparent for catching
						if ( $wasCatched ) {
							$code .= $inside;						# middle part of print() statement
						} else {
							$code .= "print( ${inside}";			# start of print() statement
						}
					} elsif ( $commandMode ) {
						# commandMode is transparent for catching also
						$command .= $inside;
					} else {
						$code .= $inside;
					}
					$catching = 1;									# catching is started or continued
					StartOB();
				} else {
					if ( $echoMode ) {
						if ( $wasCatched ) {
							$code .= " );\n";						# end of print() statement
						} else {
							$code .= "print( ${inside} );\n";
						}
						$echoMode = 0;
					} elsif ( $commandMode ) {
						$command .= $inside;
						ProcessCommand( $command );
						$commandMode = 0;
						$command = "";
					} else {
						$code .= $inside;
					}
				}
				redo OPENING;				# NB: redo jumps to the beginning of the inner block
			} else {
				$inside .= $_;
				$_ = <$f>;					# continue looking for $TAG_CLOSE
				goto CLOSING;
			};
		} else {
			print $_;
		}
	}
	if ( $catching ) {
		die "Unfinished catching.";
	}
	if ( $CGuard ) {
		print "\n#endif		// ${CGuard}\n";
	}

	OutputPlain();
	EndOB();
	close( $f ) or die $!;
}

sub Main {
	my $f;
	my $argEval = "";
	my $argDebug = 0;
	my $inputFilename = "";
	my $outputFilename = "";

	while ( my $a = shift ) {
		if ( $a eq "--eval" ) {
			$argEval .= ( shift ) || "";
		} elsif ( $a eq "--debug" ) {
			$argDebug = 1;
		} elsif ( $a eq "--output" ) {
			$outputFilename = ( shift ) || "";
		} else {
			$inputFilename = $a;
		}
		# TODO parameters to the processed script
	}

	$package = $inputFilename;
	$package =~ s/^([a-zA-Z_][a-zA-Z_0-9.]*).p$/$1/;
	$package =~ s/[.\/\\]/_/g;

	$code = "package PPP_${package}; use strict; use warnings;\n${argEval}\n";
	ParseFile( $inputFilename );
	if ( $argDebug ) {
		print STDERR $code;
	} else {
		if ( $outputFilename ) {
			open( $f, ">", $outputFilename ) or die $!;
		} else {
			open( $f, ">&STDOUT" ) or die $!;
		}
		StartOB();
		eval( $code ); warn $@ if $@;
		print $f EndOB();
		close( $f ) or die $!;
	}
}

Main( @ARGV );
