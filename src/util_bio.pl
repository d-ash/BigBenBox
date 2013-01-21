#
# Generates one .h and .c files from .bio definition.
#

use strict;
use warnings;
use Data::Dumper;

my @records = ();
my $errors = 0;
my $line = 0;
my $filename = "";
my $namespace = "";

sub Perr {
	my $s = shift;

	print "line $line: " . $s . "\n";
	$errors += 1;
}

sub ReadBIO {
	my $SYMBOL = "[a-zA-Z_][a-zA-Z_0-9]*";
	my $COMMENTS = "\\s*(?:\\/\\/.*)?";
	my $inRecord = 0;
	my $f;

	$errors = 0;
	$line = 0;

	open $f, $filename or die $!;

	for ( <$f> ) {
		$line += 1;
		if ( $_ =~ /^($SYMBOL)\s+\{$COMMENTS$/ ) {
			# record definition (begin)
			if ( $inRecord ) {
				Perr "The record ( $1 ) is inside another one.";
			} else {
				$inRecord = 1;
				push( @records, { "record" => $1, "fields" => [] } );
			}
		} elsif ( $_ =~ /^\t($SYMBOL)\s+($SYMBOL);$COMMENTS$/ ) {
			# field definition
			if ( $inRecord ) {
				push( @{ $records[ $#records ]{ "fields" } }, {
					"type" => $1,
					"name" => $2
				} );
			} else {
				Perr "The field ( $1 $2 ) is outside of records.";
			}
		} elsif ( $_ =~ /^\};$COMMENTS$/ ) {
			# record definition (end)
			if ( $inRecord ) {
				$inRecord = 0;
			} else {
				Perr "Unexpected end of a record.";
			}
		} elsif ( $_ =~ /^$COMMENTS$/ ) {
			# skipping
		} else {
			Perr "Corrupted line.";
		}
	}

	close $f or die $!;
}

sub WriteH {
	my $f;
	my $def = "_" . uc( $namespace ) . "_BIO_H";

	open $f, ">", "$filename.h" or die $!;

	print $f "// This file is automatically generated by util_bio.pl\n";
	print $f "//   source: $filename\n";
	print $f "//   namespace: $namespace\n\n";
	print $f "#ifndef $def\n";
	print $f "#define $def\n\n";

	foreach ( @records ) {
		my %rec = %$_;
		my $full = "${namespace}_" . $rec{ "record" };

		print $f "typedef struct {\n";
		foreach ( @{ $rec{ "fields" } } ) {
			my %fld = %$_;

			print $f "\t" . $fld{ "type" } . "\t\t" . $fld{ "name" } . ";\n";
		}
		print $f "} ${full}_t;\n\n";

		$_->{ "protoRead" } = sprintf "size_t ${full}_Read( ${full}_t* const r )";
		$_->{ "protoWrite" } = sprintf "size_t ${full}_Write( const ${full}_t* const r )";

		print $f $_->{ "protoRead" } . ";\n";
		print $f $_->{ "protoWrite" } . ";\n";

		print $f "\n";
	}

	print $f "#endif\n";
	close $f or die $!;
}

sub WriteC {
	my $f;

	open $f, ">", "$filename.c" or die $!;

	print $f "// This file is automatically generated by util_bio.pl\n";
	print $f "//   source: $filename\n";
	print $f "//   namespace: $namespace\n\n";
	print $f "#include \"$filename.h\"\n\n";

	foreach ( @records ) {
		my %rec = %$_;
		my $full = "${namespace}_" . $rec{ "record" };

		print $f $rec{ "protoRead" } . " {\n";
		print $f "\t// TODO\n";
		print $f "}\n\n";

		print $f $rec{ "protoWrite" } . " {\n";
		print $f "\t// TODO\n";
		print $f "}\n\n";
	}

	close $f or die $!;
}

$filename = shift or die "Usage: perl util_bio.pl < bioFilename >\n";
if ( $filename =~ /^([a-z][a-z_0-9]+)\.bio$/ ) {
	$namespace = $1;
} else {
	die "util_bio.pl: Input file has a strange extension (must be .bio)\n";
}

ReadBIO;

if ( $errors > 0 ) {
	die "*** $errors errors.\n";
}

WriteH;
WriteC;
