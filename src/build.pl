#!/usr/bin/env perl

use v5.10;
use strict;
use warnings;

# TODO gcc -M (files dependency auto-generation)

my @actions = ();
my %phonies = ();
my $_debug = 0;
my $_simulated = 0;

sub Log {
	if ( $_debug ) {
		say shift;
	}
}

sub AddAction {
	my $in = shift;
	my $out = shift;
	my $sub = shift;

	Log( join( " ", @$out ) . " <--- " . join( " ", @$in ) );
	return ( push( @actions, [ $in, $out, $sub, 0 ] ) - 1 );
}

sub SetPhony {
	my $name = shift;
	my $id = shift;

	$phonies{ $name } = $id;
}

sub FindCreator {
	my $fname = shift;
	my $i;
	my @r;

	for $i ( 0 .. $#actions ) {
		@r = grep( /^${fname}$/, @{$actions[ $i ][ 1 ]} );
		if ( scalar @r == 1 ) {
			return $i;				# TODO assuming that only one creator was defined
		} elsif ( scalar @r == 0 ) {
			next;
		} else {
			die "Duplicated outputs";
		}
	}
	return -1;
}

sub TargetFile {
	my $fname = shift;
	my $creator;

	Log( "====> ${fname}" );
	$creator = FindCreator( $fname );
	if ( $creator == -1 ) {
		if ( ! -e $fname ) {
			die "File '${fname}' does not exist and cannot be generated";
		}
	} else {
		ProcessAction( $creator );
	}
}

sub System {
	my $cmd = shift;

	if ( substr( $cmd, 0, 1 ) eq "\@" ) {
		$cmd = substr( $cmd, 1 );
	} else {
		say "${cmd}";
	}
	if ( !$_simulated ) {
		if ( system( $cmd ) != 0 ) {
			die "system() failed";
		}
	}
}

sub ProcessAction {
	my $id = shift;
	my $act = $actions[ $id ];
	my $fname;

	if ( $act->[ 3 ] > 0 ) {
		return;
	}

	foreach $fname ( @{$act->[ 0 ]} ) {
		TargetFile( $fname );
	}

	if ( $act->[ 2 ] ne "") {
		System( $act->[ 2 ] );
	}
	$act->[ 3 ]++;
}

sub TargetPhony {
	my $name = shift;

	ProcessAction( $phonies{ $name } );
}

############################################################################

#$_debug = 1;
#$_simulated = 1;

my $LIB_NAME	= "bbb";
my $RELEASE		= 0;
my $VERBOSE		= 1;
my $PLATFORM	= "LINUX";

my $BUILD_DIR;
if ( $RELEASE ) {
	$BUILD_DIR	= "../build/release";
} else {
	$BUILD_DIR	= "../build/dev";
}
my $GEN_DIR		= ".gen";
my $LIB_FILENAME	= "${BUILD_DIR}/lib${LIB_NAME}.a";

sub PerlPP {
	my $inFile = shift;
	my $outFile = $inFile;

	$outFile =~ s/^(.+)\.p$/$GEN_DIR\/$1/;
	return ( [ $inFile ], [ $outFile ], "perlpp -o $outFile $inFile" );
}

sub BioH {
	my $inFile = shift;
	my $outFile = $inFile;

	$outFile =~ s/^(.+)\.p$/$GEN_DIR\/$1.h/;
	return ( [ $inFile, "bio.p" ], [ $outFile ], "perlpp -e '\$DEF{ \"isHeader\" } = 1;' -o $outFile $inFile" );
}

sub BioC {
	my $inFile = shift;
	my $outFile = $inFile;
	my $hFile = $inFile;

	$outFile =~ s/^(.+)\.p$/$GEN_DIR\/$1.c/;
	$hFile =~ s/^(.+)\.p$/$1.h/;
	return ( [ $inFile, "bio.p" ], [ $outFile ], "perlpp -e '\$DEF{ \"isHeader\" } = 0; \$DEF{ \"hFilename\" } = \"$hFile\";' -o $outFile $inFile" );
}

sub Compile {
	my $basename = shift;
	my $insideLib = shift || 0;
	my $cFile = "${GEN_DIR}/${basename}.c";
	my $oFile = "${BUILD_DIR}/${basename}.o";
	my $flags = "-Wall -Wextra -I${GEN_DIR} -DBBB_PLATFORM_${PLATFORM} -c";

	if ( $insideLib ) {
		$flags .= " -DBBB_INSIDE_LIB";
	}
	if ( $VERBOSE ) {
		$flags .= " -DBBB_VERBOSE";
	}
	if ( $RELEASE ) {
		$flags .= " -DBBB_RELEASE";
	} else {
		$flags .= " -ggdb";
	}
	return ( [ $cFile ], [ $oFile ], "gcc ${flags} -o ${oFile} $cFile" );
}

sub Link {
	my $basename = shift;
	my $eFile = "${BUILD_DIR}/${basename}";
	my $oFile = "${BUILD_DIR}/${basename}.o";
	my $flags = "-Wall -Wextra -L${BUILD_DIR} -lssl -lcrypto -l${LIB_NAME}";

	if ( !$RELEASE ) {
		$flags .= " -ggdb";
	}
	return ( [ $oFile, $LIB_FILENAME ], [ $eFile ], "gcc -o $eFile $oFile ${flags}" );
}

# ============= Library ==============

my @libraryObjFiles = map { "${BUILD_DIR}/$_" } qw(
	bbb_errors.o
	bbb_bio.o
	bbb_sshot.o
	bbb_sshot_file.o
	bbb_sshot_file.bio.o
	bbb_util.o
	bbb_util_hash.o
);
my @libraryHdrFiles =
	map { my $i = $_; $i =~ s/^\Q${BUILD_DIR}\E\/(.+).o$/$GEN_DIR\/$1.h/; $i; } @libraryObjFiles;
push( @libraryHdrFiles, "${GEN_DIR}/bbb.h" );

SetPhony( "library", AddAction(
	\@libraryObjFiles,
	[ $LIB_FILENAME ],
	"ar -r ${LIB_FILENAME} " . join( " ", @libraryObjFiles ) ) );
SetPhony( "libraryHeaders", AddAction( \@libraryHdrFiles, [], "" ) );

foreach ( @libraryObjFiles ) {
	my $i = $_;
	$i =~ s/^\Q${BUILD_DIR}\/\E(.+)\.o$/$1/;
	AddAction( Compile( $i, 1 ) );
	if ( $i =~ m/^(.+\.bio)$/ ) {
		AddAction( BioH( "$1.p" ) );
		AddAction( BioC( "$1.p" ) );
	} else {
		AddAction( PerlPP( "$i.h.p" ) );
		AddAction( PerlPP( "$i.c.p" ) );
	}
}

AddAction( PerlPP( "bbb.h.p" ) );

# ============== Client ===============

SetPhony( "clientRun", AddAction( [], [], "\@cd ${BUILD_DIR}/client && ./client" ) );
SetPhony( "client", AddAction( Link( "client/client" ) ) );
SetPhony( "clientHeaders", AddAction( [ "${GEN_DIR}/client/client.h" ], [], "" ) );
AddAction( Compile( "client/client" ) );
AddAction( PerlPP( "client/client.h.p" ) );
AddAction( PerlPP( "client/client.c.p" ) );

# =============== Tests ===============

my @tests = map { s/\n$//; "tests/$_"; } `perl tests/runner.pl`;
my $biotest = "tests/test_bio.bio";
my @testsExeFiles = map { "${BUILD_DIR}/$_"; } @tests;
push( @testsExeFiles, "tests/runner.pl" );
my @testsHdrFiles = map { "${GEN_DIR}/$_.h"; } @tests;
push( @testsHdrFiles, "${GEN_DIR}/${biotest}.h" );
push( @testsHdrFiles, "${GEN_DIR}/${biotest}.c" );
push( @testsHdrFiles, "${GEN_DIR}/tests/minunit.h" );

SetPhony( "testsRun", AddAction( [], [], "\@cd ${BUILD_DIR}/tests && perl runner.pl run" ) );
SetPhony( "tests", AddAction( \@testsExeFiles, [], "cp tests/runner.pl ${BUILD_DIR}/tests/runner.pl" ) );
SetPhony( "testsHeaders", AddAction( \@testsHdrFiles, [], "" ) );

foreach ( @tests ) {
	my $i = $_;
	AddAction( Link( $_ ) );
	AddAction( Compile( $_ ) );
	AddAction( PerlPP( "$_.h.p" ) );
	AddAction( PerlPP( "$_.c.p" ) );
}
AddAction( BioH( "${biotest}.p" ) );
AddAction( BioC( "${biotest}.p" ) );
AddAction( PerlPP( "tests/minunit.h.p" ) );

# ====================================================================================

SetPhony( "clean", AddAction( [], [], "rm -rf ${BUILD_DIR}/* && rm -rf ${GEN_DIR}/*" ) );
SetPhony( "makeTestsDirs", AddAction( [], [], "mkdir -p ${BUILD_DIR}/tests && mkdir -p ${GEN_DIR}/tests" ) );
SetPhony( "makeClientDirs", AddAction( [], [], "mkdir -p ${BUILD_DIR}/client && mkdir -p ${GEN_DIR}/client" ) );

TargetPhony( "clean" );
TargetPhony( "makeTestsDirs" );
TargetPhony( "makeClientDirs" );

TargetPhony( "libraryHeaders" );
TargetPhony( "library" );

TargetPhony( "clientHeaders" );
TargetPhony( "client" );

TargetPhony( "testsHeaders" );
TargetPhony( "tests" );

TargetPhony( "clientRun" );
TargetPhony( "testsRun" );
