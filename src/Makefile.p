<?
    my $LIB_NAME = "bigbenbox";
    my $RELEASE = 0;
    my $DEBUG = 0;

    my $BUILD_DIR;
    my $LIB_FILENAME;
    my $COMPILER_FLAGS;
    my $LINKER_FLAGS;

    if ( $RELEASE ) {
        $BUILD_DIR = "../build/release";
    } else {
        $BUILD_DIR = "../build/dev";
    }

    if ( $PLATFORM eq "LINUX" ) {
        $LIB_FILENAME = "${BUILD_DIR}/lib${LIB_NAME}.a";
        $COMPILER_FLAGS = "-Wall -Wextra -I./ -DBBB_PLATFORM_LINUX";
        if ( $DEBUG ) {
            $COMPILER_FLAGS .= " -DBBB_DEBUG";
        }
        if ( !$RELEASE ) {
            $COMPILER_FLAGS .= " -ggdb";
        }
        $LINKER_FLAGS = "-Wall -Wextra -L${BUILD_DIR} -lssl -lcrypto -l${LIB_NAME}";
    } else {
        die "Unknown PLATFORM.\n";
    }

    # ================ Library ================

    my @LIB_SOURCE_FILES = qw(
        bio
        sshot
        sshot_file
        sshot_file.bio
        util
        util_hash
    );

    my $CODEGEN_DIR = "generated";

    sub asString {
        return join( " ", @_ );
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
        return ( "${BUILD_DIR}/" . shift . ".o" );
    }

    my @LIB_C_FILES = map { cDst( $_ ); } @LIB_SOURCE_FILES;
    my @LIB_H_FILES = map { hDst( $_ ); } @LIB_SOURCE_FILES;
    my @LIB_O_FILES = map { oDst( $_ ); } @LIB_SOURCE_FILES;
?>

.PHONY: all makefiles delimiter directories clean

all: delimiter clean directories <?= $LIB_FILENAME ?>

makefiles:
	perl tools/perlpp.pl --comments "hash" --eval 'my $$PLATFORM = "LINUX";' Makefile.p Makefile_linux
	perl tools/perlpp.pl --comments "hash" --eval 'my $$PLATFORM = "WINDOWS";' Makefile.p Makefile_windows
	perl tools/perlpp.pl --comments "hash" --eval 'my $$PLATFORM = "OSX";' Makefile.p Makefile_osx

delimiter:
	@echo "========================"

directories:
	mkdir -p <?= $CODEGEN_DIR ?>
	mkdir -p <?= $BUILD_DIR ?>

clean:
	rm -rf <?= $CODEGEN_DIR ?>
	rm -rf <?= $BUILD_DIR ?>

<?= $LIB_FILENAME ?>: <?= asString @LIB_C_FILES ?> <?= asString @LIB_H_FILES ?> <?= asString @LIB_O_FILES ?>
	ar -rv <?= $LIB_FILENAME ?> <?= asString @LIB_O_FILES ?>

<? foreach ( @LIB_SOURCE_FILES ) { ?>

<?= oDst( $_ ) ?>: <?= cDst( $_ ) ?> <?= hDst( $_ ) ?> <?= hDst( "global" ) ?>
	gcc -c <?= $COMPILER_FLAGS ?> $< -o $@

    <? if ( m/\.bio$/ ) { ?>

<?= cDst( $_ ) ?>: <?= $_ ?>
	perl tools/bio.pl $< <?= $CODEGEN_DIR ?>
<?= hDst( $_ ) ?>: <?= cDst( $_ ) ?>
    # do nothing

    <? } else { ?>

<?= cDst( $_ ) ?>: <?= cpSrc( $_ ) ?>
	perl tools/perlpp.pl --comments "doubleslash" $< $@
<?= hDst( $_ ) ?>: <?= hpSrc( $_ ) ?>
	perl tools/perlpp.pl --comments "doubleslash" $< $@

    <? } ?>

<? } ?>

<?= hDst( "global" ) ?>: <?= hpSrc( "global" ) ?>
	perl tools/perlpp.pl --comments "doubleslash" $< $@
