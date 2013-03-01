<?

my $bio_ns;				# shortcut

sub GetPrototypes {
	my $n = shift;
	my $t = shift;
	my %protos = ();

	$protos{ "Copy" } =
		"?>int		<?= $bio_ns ?>_Copy_<?= $n ?>				( <?= $t ?>* const dst, const <?= $t ?>* const src )<?";
	$protos{ "IsEqual" } =
		"?>int		<?= $bio_ns ?>_IsEqual_<?= $n ?>			( const <?= $t ?>* const r1, const <?= $t ?>* const r2 )<?";
	$protos{ "GetSize" } =
		"?>size_t	<?= $bio_ns ?>_GetSize_<?= $n ?>			( const <?= $t ?>* const r )<?";
	$protos{ "GetSizeArray" } =
		"?>size_t	<?= $bio_ns ?>_GetSizeArray_<?= $n ?>		( const <?= $t ?>* const a, size_t const n )<?";

	$protos{ "ReadFromBuf" } =
		"?>size_t	<?= $bio_ns ?>_ReadFromBuf_<?= $n ?>		( <?= $t ?>* const r, const bbb_byte_t* const buf, const size_t len )<?";
	$protos{ "ReadFromBufArray" } =
		"?>size_t	<?= $bio_ns ?>_ReadFromBufArray_<?= $n ?>	( <?= $t ?>* const a, size_t const n, const bbb_byte_t* const buf, const size_t len )<?";
	$protos{ "WriteToBuf" } =
		"?>size_t	<?= $bio_ns ?>_WriteToBuf_<?= $n ?>			( const <?= $t ?>* const r, bbb_byte_t* const buf, const size_t len )<?";
	$protos{ "WriteToBufArray" } =
		"?>size_t	<?= $bio_ns ?>_WriteToBufArray_<?= $n ?>	( const <?= $t ?>* const a, size_t const n, bbb_byte_t* const buf, const size_t len )<?";

	$protos{ "ReadFromFile" } =
		"?>size_t	<?= $bio_ns ?>_ReadFromFile_<?= $n ?>		( <?= $t ?>* const r, FILE* const f, bbb_checksum_t* const chk )<?";
	$protos{ "ReadFromFileArray" } =
		"?>size_t	<?= $bio_ns ?>_ReadFromFileArray_<?= $n ?>	( <?= $t ?>* const a, size_t const n, FILE* const f, bbb_checksum_t* const chk )<?";
	$protos{ "WriteToFile" } =
		"?>size_t	<?= $bio_ns ?>_WriteToFile_<?= $n ?>		( const <?= $t ?>* const r, FILE* const f, bbb_checksum_t* const chk )<?";
	$protos{ "WriteToFileArray" } =
		"?>size_t	<?= $bio_ns ?>_WriteToFileArray_<?= $n ?>	( const <?= $t ?>* const a, size_t const n, FILE* const f, bbb_checksum_t* const chk )<?";

	$protos{ "Destroy" } =
		"?>void		<?= $bio_ns ?>_Destroy_<?= $n ?>			( <?= $t ?>* const r )<?";
	$protos{ "DestroyEach" } =
		"?>void		<?= $bio_ns ?>_DestroyEach_<?= $n ?>		( <?= $t ?>* const a, size_t const n )<?";

	return \%protos;
}

sub WriteRecordToH {
	my $recName = shift;
	my $fields = shift;
	my $recType = "${bio_ns}_${recName}_t";
	my $params = {
		"isBytesOnly"	=> 1,
		"isDynamic"		=> 0
	};
	my $protos;
	my $i;

	?> typedef struct { <?/
	foreach $i ( @$fields ) {
		if ( $i->{ "type" } eq "varbuf" ) {
			$params->{ "isDynamic" } = 1;
			$params->{ "isBytesOnly" } = 0;
			?> bbb_varbuf_t				<?= $i->{ "name" } ?>; <?/
			next;
		}
		if ( $i->{ "type" } ne "uint8" ) {
			$params->{ "isBytesOnly" } = 0;
		}
		?> <?= $i->{ "type" } ?>_t		<?= $i->{ "name" } ?>; <?/
	}
	?> } <?= $recType ?>; <?/

	$protos = GetPrototypes( $recName, $recType );
	foreach $i ( sort( keys %$protos ) ) {
		?> <?= $protos->{ $i } ?>; <?/
	}

	return ( $params, $protos );
}

sub IsAtomType {
	return ( shift =~ m/^uint(?:8|16|32|64)$/ );
}

sub Output_Assert {
    my $_;
	my $ifexp = shift;
	my $fields = shift;

	print "	if ( ${ifexp} ) {\n";
	foreach ( @{$fields} ) {
		print "		free( r->$_.buf ); r->$_.buf = NULL; r->$_.len = 0;\n";
	}
	print "		return 0;\n";
	print "	}\n";
}

sub Output_ReadImpl {
	my $mode = shift;
	my $fields = shift;
	my $params = shift;
	my $protos = shift;
	my @allocated = ();		# collection of fields with dynamically allocated memory
	my $i;
	my $type;				# type of a field
	my $name;				# name of a field

	?> <?= $protos->{ "ReadFrom${mode}" } ?> {
		size_t	cur = 0;
		<?
		if ( !$params->{ "isBytesOnly" } ) {
			?> size_t	red; <?/
		}
		foreach $i ( @$fields ) {
			$type = $i->{ "type" };
			$name = $i->{ "name" };

			if ( $type eq "uint8" ) {
				if ( $mode eq "Buf" ) {
					Output_Assert( "cur >= len", \@allocated );
					?> r-><?= $name ?> = *( buf + cur ); <?/
				} else {
					Output_Assert( "fread( &( r->${name} ), 1, 1, f ) == 0", \@allocated );
					?> bbb_util_hash_UpdateChecksum( &( r-><?= $name ?> ), 1, chk ); <?/
				}
				?> cur++; <?/
			} else {
				if ( $mode eq "Buf" ) {
					?> red = bbb_bio_ReadFrom<?= $mode ?>_<?= $type ?>( &( r-><?= $name ?> ), buf + cur, len - cur ); <?/
				} else {
					?> red = bbb_bio_ReadFrom<?= $mode ?>_<?= $type ?>( &( r-><?= $name ?> ), f, chk ); <?/
				}
				Output_Assert( "red == 0", \@allocated );
				?> cur += red; <?/
				if ( $type eq "varbuf" ) {
					push( @allocated, $name );
				}
			}
		}
		?>
		return cur;
	}
	<?/
}

sub Output_ReadArrayImpl {
	my $mode = shift;
	my $recName = shift;
	my $fields = shift;
	my $params = shift;
	my $protos = shift;

	?> <?= $protos->{ "ReadFrom${mode}Array" } ?> {
		size_t	i;
		size_t	cur = 0;
		size_t	red;
		for ( i = 0; i < n; i++ ) { <?/
			if ( $mode eq "Buf" ) {
				?> red = <?= $bio_ns ?>_ReadFrom<?= $mode ?>_<?= $recName ?>( &( a[ i ] ), buf + cur, len - cur ); <?/
			} else {
				?> red = <?= $bio_ns ?>_ReadFrom<?= $mode ?>_<?= $recName ?>( &( a[ i ] ), f, chk ); <?/
			}
			?> if ( red == 0 ) { <?/
				if ( $params->{ "isDynamic" } ) {
					?> <?= $bio_ns ?>_DestroyEach_<?= $recName ?>( a, i ); <?/
				}
				?>
				return 0;
			}
			cur += red;
		}
		return cur;
	}
	<?/
}

sub Output_WriteImpl {
	my $mode = shift;
	my $fields = shift;
	my $params = shift;
	my $protos = shift;
	my $i;
	my $type;				# type of a field
	my $name;				# name of a field

	?> <?= $protos->{ "WriteTo${mode}" } ?> {
		size_t	cur = 0;
		<?
		if ( !$params->{ "isBytesOnly" } ) {
			?> size_t	wtn; <?/
		}
		foreach $i ( @$fields ) {
			my $type = $i->{ "type" };
			my $name = $i->{ "name" };

			if ( $type eq "uint8" ) {
				if ( $mode eq "Buf" ) {
					?>
					if ( cur >= len ) { return 0; }
					*( buf + cur ) = r-><?= $name ?>;
					<?
				} else {
					?>
					bbb_util_hash_UpdateChecksum( &( r-><?= $name ?> ), 1, chk );
					if ( fwrite( &( r-><?= $name ?> ), 1, 1, f ) == 0 ) { return 0; }
					<?
				}
				?> cur++; <?/
			} else {
				if ( $mode eq "Buf" ) {
					?> wtn = bbb_bio_WriteTo<?= $mode ?>_<?= $type ?>( r-><?= $name ?>, buf + cur, len - cur ); <?/
				} else {
					?> wtn = bbb_bio_WriteToFile_<?= $type ?>( r-><?= $name ?>, f, chk ); <?/
				}
				?>
				if ( wtn == 0 ) { return 0; }
				cur += wtn;
				<?
			}
		}
		?>
		return cur;
	}
	<?/
}

sub Output_WriteArrayImpl {
	my $mode = shift;
	my $recName = shift;
	my $fields = shift;
	my $params = shift;
	my $protos = shift;

	?> <?= $protos->{ "WriteTo${mode}Array" } ?> {
		size_t	i;
		size_t	cur = 0;
		size_t	wtn;
		for ( i = 0; i < n; i++ ) { <?/
			if ( $mode eq "Buf" ) {
				?> wtn = <?= $bio_ns ?>_WriteTo<?= $mode ?>_<?= $recName ?>( &( a[ i ] ), buf + cur, len - cur ); <?/
			} else {
				?> wtn = <?= $bio_ns ?>_WriteTo<?= $mode ?>_<?= $recName ?>( &( a[ i ] ), f, chk ); <?/
			}
			?>
			if ( wtn == 0 ) { return 0; }
			cur += wtn;
		}
		return cur;
	}
	<?/
}

sub WriteRecordToC {
	my $recName = shift;
	my $fields = shift;
	my $params = shift;
	my $protos = shift;
	my $i;
	my $type;				# type of a field
	my $name;				# name of a field

	?> <?= $protos->{ "Copy" } ?> { <?/
		if ( $params->{ "isDynamic" } ) {
			?> size_t	len; <?/
		}
		foreach $i ( @$fields ) {
			$type = $i->{ "type" };
			$name = $i->{ "name" };

			if ( IsAtomType $type ) {
				?> dst-><?= $name ?> = src-><?= $name ?>; <?/
			} elsif ( $type eq "varbuf" ) {
				?>
				len = dst-><?= $name ?>.len = src-><?= $name ?>.len;
				if ( BBB_FAILED( bbb_util_Malloc( ( void** )&( dst-><?= $name ?>.buf ), len ) ) ) {
					exit( 1 );
				}
				memcpy( dst-><?= $name ?>.buf, src-><?= $name ?>.buf, len );
				<?
			} else {
				die "Unknown field type: $type";
			}
		}
		?>
		return 1;
	}

	<?= $protos->{ "IsEqual" } ?> { <?/
		foreach $i ( @$fields ) {
			$type = $i->{ "type" };
			$name = $i->{ "name" };

			if ( IsAtomType $type ) {
				?> if ( r1-><?= $name ?> != r2-><?= $name ?> ) <?/
			} elsif ( $type eq "varbuf" ) {
				?> if ( !bbb_bio_IsEqual_varbuf( r1-><?= $name ?>, r2-><?= $name ?> ) ) <?/
			} else {
				die "Unknown field type: $type";
			}

			?> { return 0; } <?/
		}
		?>
		return 1;
	}

	<?= $protos->{ "GetSize" } ?> {
		return ( 0 <?
		foreach $i ( @$fields ) {
			$type = $i->{ "type" };
			$name = $i->{ "name" };

			?> + <?
			if ( IsAtomType $type ) {
				?> sizeof( r-><?= $name ?> ) <?
			} elsif ( $type eq "varbuf" ) {
				?> sizeof( r-><?= $name ?>.len ) + r-><?= $name ?>.len <?
			} else {
				die "Unknown field type: $type";
			}
		}
		?> );
	}

	<?= $protos->{ "GetSizeArray" } ?> {
		size_t	i;
		size_t	sz = 0;
		for ( i = 0; i < n; i++ ) {
			sz += <?= $bio_ns ?>_GetSize_<?= $recName ?>( &( a[ i ] ) );
		}
		return sz;
	}

<?
	Output_ReadImpl( "Buf", $fields, $params, $protos );
	Output_ReadArrayImpl( "Buf", $recName, $fields, $params, $protos );
	Output_WriteImpl( "Buf", $fields, $params, $protos );
	Output_WriteArrayImpl( "Buf", $recName, $fields, $params, $protos );

	Output_ReadImpl( "File", $fields, $params, $protos );
	Output_ReadArrayImpl( "File", $recName, $fields, $params, $protos );
	Output_WriteImpl( "File", $fields, $params, $protos );
	Output_WriteArrayImpl( "File", $recName, $fields, $params, $protos );
?>

	<?= $protos->{ "Destroy" } ?> { <?/
		if ( $params->{ "isDynamic" } ) {
			foreach $i ( @$fields ) {
				$type = $i->{ "type" };
				$name = $i->{ "name" };

				if ( $type eq "varbuf" ) {
					?>
					free( r-><?= $name ?>.buf );
					r-><?= $name ?>.buf = NULL;
					r-><?= $name ?>.len = 0;
					<?
				}
			}
		} else {
			?> ( void ) r; <?/
		}
	?>
	}

	<?= $protos->{ "DestroyEach" } ?> { <?/
		if ( $params->{ "isDynamic" } ) {
			?>
			size_t	i;
			for ( i = 0; i < n; i++ ) {
				<?= $bio_ns ?>_Destroy_<?= $recName ?>( &( a[ i ] ) );
			}
			<?
		} else {
			?>
			( void ) a;
			( void ) n;
			<?
		}
	?>
	}
	<?
}	##### end of WriteRecordToC

sub bio_Start {
	$bio_ns = $DEF{ "namespace" };
	if ( $DEF{ "isHeader" } ) {
		?>
#ifndef _<?= uc( $bio_ns ) ?>_BIO_H_FILE
#define _<?= uc( $bio_ns ) ?>_BIO_H_FILE
#include "bbb_bio.h"
		<?
	} else {
		?>
#include "<?= $DEF{ "hFilename" } ?>"
#include "bbb_util_hash.h"
		<?
	}
}

sub bio_End {
	if ( $DEF{ "isHeader" } ) {
		?>
#endif
		<?
	}
}

sub bio_CreateRecord {
	my $name = shift;
	my $fields = shift;
	my $params;
	my $protos;

	if ( $DEF{ "isHeader" } ) {
		WriteRecordToH( $name, $fields );
	} else {
		PerlPP::StartOB();
		( $params, $protos ) = WriteRecordToH( $name, $fields );	# we need only returned values
		PerlPP::EndOB();
		WriteRecordToC( $name, $fields, $params, $protos );
	}
}

?>
