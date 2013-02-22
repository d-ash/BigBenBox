<?  # vim: filetype=make

	# TODO smart dependency generation

    my $LIB_NAME = "bigbenbox";
    my $RELEASE = 0;
    my $DEBUG = 1;

    my @LIB_SOURCE_FILES = qw(
        bio
        sshot
        sshot_file
        sshot_file.bio
        util
        util_hash
    );

    my @CLT_SOURCE_FILES = qw(
        client
    );

	my $TST_RUNNER = "tests/runner.pl";
	my @TST_SOURCE_FILES = map { s/\n$//; "tests/$_"; } `perl $TST_RUNNER`;

	# =========================================

    my $BUILD_DIR;
    my $LIB_FILENAME;
    my $CLT_FILENAME;
    my $COMPILER_FLAGS;
    my $LINKER_FLAGS;
    my $CODEGEN_DIR = "generated";

    my $extObj = ".o";
    my $extExe = "";

    if ( $RELEASE ) {
        $BUILD_DIR = "../build/release";
    } else {
        $BUILD_DIR = "../build/dev";
    }

    sub cpSrc {
        return ( shift . ".c.p" );
    }

    sub hpSrc {
        return ( shift . ".h.p" );
    }

    sub cDst {
        return ( "${CODEGEN_DIR}/" . shift . ".c" );
    }

    sub hDst {
        return ( "${CODEGEN_DIR}/" . shift . ".h" );
    }

    sub oDst {
        return ( "${BUILD_DIR}/" . shift . $extObj );
    }

    sub exeDst {
        return ( "${BUILD_DIR}/" . shift . $extExe );
    }

	sub perlppCmd { ?>perlpp -o $@ $<<? }

	sub bioCmd { ?>perl tools/bio.pl --output-dir <?= $CODEGEN_DIR ?> $<<? }

    sub PreprocessAndCompile {
		my $_;

        foreach ( @_ ) {
?>

<?= oDst( $_ ) ?>: <?= cDst( $_ ) ?> <?= hDst( "global" ) ?> <?= hDst( "bigbenbox" ) ?>
	gcc -c <?= $COMPILER_FLAGS ?> $< -o $@

    <? if ( m/\.bio$/ ) { ?>

<?= hDst( $_ ) ?>: <?= cDst( $_ ) ?>
<?= cDst( $_ ) ?>: <?= $_ ?>
	<? bioCmd(); ?>

    <? } else { ?>

<?= cDst( $_ ) ?>: <?= cpSrc( $_ ) ?>
	<? perlppCmd(); ?>
<?= hDst( $_ ) ?>: <?= hpSrc( $_ ) ?>
	<? perlppCmd(); ?>

    <? } ?>

<?
        }
    }

    if ( ( $PLATFORM eq "LINUX" ) or ( $PLATFORM eq "OSX" ) ) {
        $LIB_FILENAME = "${BUILD_DIR}/lib${LIB_NAME}.a";
        $CLT_FILENAME = "${BUILD_DIR}/client";
        $COMPILER_FLAGS = "-Wall -Wextra -I${CODEGEN_DIR} -DBBB_PLATFORM_${PLATFORM}";
        if ( $DEBUG ) {
            $COMPILER_FLAGS .= " -DBBB_DEBUG";
        }
        if ( !$RELEASE ) {
            $COMPILER_FLAGS .= " -ggdb";
        }
        $LINKER_FLAGS = "-Wall -Wextra -L${BUILD_DIR} -lssl -lcrypto -l${LIB_NAME}";
	} elsif ( $PLATFORM eq "WINDOWS" ) {
		$extObj = ".obj";
		$extExe = ".exe";
    } else {
        die "Unknown PLATFORM.\n";
    }

    my $LIB_O_FILES = join( " " , map { oDst( $_ ); } @LIB_SOURCE_FILES );
    my $CLT_O_FILES = join( " ", map { oDst( $_ ); } @CLT_SOURCE_FILES );
    my $TST_FILENAMES = join( " ", map { exeDst( $_ ); } @TST_SOURCE_FILES );

	# Let's process them before anything else.
	my @INCLUDES = map { hDst( $_ ); } (
		@LIB_SOURCE_FILES,
		@CLT_SOURCE_FILES,
		@TST_SOURCE_FILES
	);
	push @INCLUDES, (
			hDst( "tests/minunit" ),
			hDst( "tests/test_bio.bio" ),
			cDst( "tests/test_bio.bio" )
		);
?>

.PHONY: all makefiles delimiter clean directories includes library client tests run runtests

all: delimiter clean directories includes library client tests

makefiles:
	perlpp -e 'my $$PLATFORM = "LINUX";' -o Makefile_linux Makefile.p
	perlpp -e 'my $$PLATFORM = "WINDOWS";' -o Makefile_windows Makefile.p
	perlpp -e 'my $$PLATFORM = "OSX";' -o Makefile_osx Makefile.p

delimiter:
	@echo "========================"

directories:
	mkdir -p <?= $CODEGEN_DIR ?>
	mkdir -p <?= $BUILD_DIR ?>
	mkdir -p <?= $CODEGEN_DIR ?>/tests
	mkdir -p <?= $BUILD_DIR ?>/tests

clean:
	rm -rf <?= $CODEGEN_DIR ?>
	rm -rf <?= $BUILD_DIR ?>

run:
	@cd <?= $BUILD_DIR ?>; \
		./client;

runtests:
	@cd <?= $BUILD_DIR ?>/tests; \
		perl runner.pl run;

includes: <?= join( " ", @INCLUDES ) ?>

#
# ========== Library ==========
#

library: <?= $LIB_FILENAME ?>

<?= $LIB_FILENAME ?>: <?= $LIB_O_FILES ?>
	ar -rv <?= $LIB_FILENAME ?> <?= $LIB_O_FILES ?>

<? PreprocessAndCompile( @LIB_SOURCE_FILES ); ?>

<?= hDst( "global" ) ?>: <?= hpSrc( "global" ) ?>
	<? perlppCmd(); ?>

<?= hDst( "bigbenbox" ) ?>: <?= hpSrc( "bigbenbox" ) ?>
	<? perlppCmd(); ?>

#
# ========== Client ==========
#

client: <?= $CLT_FILENAME ?>

<?= $CLT_FILENAME ?>: <?= $LIB_FILENAME ?> <?= $CLT_O_FILES ?>
	gcc <?= $CLT_O_FILES ?> <?= $LINKER_FLAGS ?> -o $@

<? PreprocessAndCompile( @CLT_SOURCE_FILES ); ?>

#
# ========== Tests ==========
#

tests: <?= $TST_RUNNER ?> <?= $TST_FILENAMES ?>
	cp $< <?= $BUILD_DIR ?>/tests/

<? foreach ( @TST_SOURCE_FILES ) { ?>
<?= exeDst( $_ ) ?>: <?= oDst( $_ ) ?> library
	gcc $< <?= $LINKER_FLAGS ?> -o $@
<? } ?>

<? PreprocessAndCompile( @TST_SOURCE_FILES ); ?>

<?= hDst( "tests/minunit" ) ?>: <?= hpSrc( "tests/minunit" ) ?>
	<? perlppCmd(); ?>

<?= hDst( "tests/test_bio.bio" ) ?>: <?= cDst( "tests/test_bio.bio" ) ?>
<?= cDst( "tests/test_bio.bio" ) ?>: <?= "tests/test_bio.bio" ?>
	<? bioCmd(); ?>

