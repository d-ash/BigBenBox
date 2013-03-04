<?:include bbb.p ?>
<?

my $bio_ns;				# shortcut

sub GetPrototypes {
	my $n = shift;
	my $t = shift;
	my %protos = ();

	$protos{ "Copy" } =
		"?>bbb_result_t	<?= $bio_ns ?>_Copy_<?= $n ?>				( <?= $t ?>* const dst, const <?= $t ?>* const src )<?";
	$protos{ "IsEqual" } =
		"?>int		<?= $bio_ns ?>_IsEqual_<?= $n ?>				( const <?= $t ?>* const r1, const <?= $t ?>* const r2 )<?";
	$protos{ "GetSize" } =
		"?>size_t	<?= $bio_ns ?>_GetSize_<?= $n ?>				( const <?= $t ?>* const r )<?";
	$protos{ "GetSizeArray" } =
		"?>size_t	<?= $bio_ns ?>_GetSizeArray_<?= $n ?>			( const <?= $t ?>* const a, size_t const n )<?";

	$protos{ "ReadFromBuf" } =
		"?>bbb_result_t	<?= $bio_ns ?>_ReadFromBuf_<?= $n ?>		( <?= $t ?>* const r, const bbb_byte_t* const buf, const size_t len )<?";
	$protos{ "ReadFromBufArray" } =
		"?>bbb_result_t	<?= $bio_ns ?>_ReadFromBufArray_<?= $n ?>	( <?= $t ?>* const a, size_t const n, const bbb_byte_t* const buf, const size_t len )<?";
	$protos{ "WriteToBuf" } =
		"?>bbb_result_t	<?= $bio_ns ?>_WriteToBuf_<?= $n ?>			( const <?= $t ?>* const r, bbb_byte_t* const buf, const size_t len )<?";
	$protos{ "WriteToBufArray" } =
		"?>bbb_result_t	<?= $bio_ns ?>_WriteToBufArray_<?= $n ?>	( const <?= $t ?>* const a, size_t const n, bbb_byte_t* const buf, const size_t len )<?";

	$protos{ "ReadFromFile" } =
		"?>bbb_result_t	<?= $bio_ns ?>_ReadFromFile_<?= $n ?>		( <?= $t ?>* const r, FILE* const f, bbb_checksum_t* const chk )<?";
	$protos{ "ReadFromFileArray" } =
		"?>bbb_result_t	<?= $bio_ns ?>_ReadFromFileArray_<?= $n ?>	( <?= $t ?>* const a, size_t const n, FILE* const f, bbb_checksum_t* const chk )<?";
	$protos{ "WriteToFile" } =
		"?>bbb_result_t	<?= $bio_ns ?>_WriteToFile_<?= $n ?>		( const <?= $t ?>* const r, FILE* const f, bbb_checksum_t* const chk )<?";
	$protos{ "WriteToFileArray" } =
		"?>bbb_result_t	<?= $bio_ns ?>_WriteToFileArray_<?= $n ?>	( const <?= $t ?>* const a, size_t const n, FILE* const f, bbb_checksum_t* const chk )<?";

	$protos{ "Destroy" } =
		"?>void		<?= $bio_ns ?>_Destroy_<?= $n ?>				( <?= $t ?>* const r )<?";
	$protos{ "DestroyEach" } =
		"?>void		<?= $bio_ns ?>_DestroyEach_<?= $n ?>			( <?= $t ?>* const a, size_t const n )<?";

	return \%protos;
}

sub WriteRecordToH {
	my $recName = shift;
	my $fields = shift;
	my $recType = "${bio_ns}_${recName}_t";
	my $params = {
		"hasBytes"		=> 0,
		"isBytesOnly"	=> 1,
		"isDynamic"		=> 0
	};
	my $protos;
	my $i;
	my $type;				# type of a field
	my $name;				# name of a field

	?> typedef struct { <?/
	foreach $i ( @$fields ) {
		$type = $i->{ "type" };
		$name = $i->{ "name" };

		if ( $type eq "varbuf" ) {
			$params->{ "isDynamic" } = 1;
			$params->{ "isBytesOnly" } = 0;
			?> bbb_varbuf_t				<?= $name ?>; <?/
			next;
		}
		if ( $type eq "uint8" ) {
			$params->{ "hasBytes" } = 1;
		} else {
			$params->{ "isBytesOnly" } = 0;
		}
		?> <?= $type ?>_t		<?= $name ?>; <?/
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

sub Output_ReadImpl {
	my $mode = shift;
	my $fields = shift;
	my $params = shift;
	my $protos = shift;
	my $i;
	my $type;				# type of a field
	my $name;				# name of a field

	?> <?= $protos->{ "ReadFrom${mode}" } ?> {
		bbb_result_t	result = BBB_SUCCESS;
		size_t			cur = 0;
		<? if ( $mode eq "File" && $params->{ "hasBytes" } ) { ?>
			size_t			wasRead;
		<? }

		foreach $i ( @$fields ) {
			$type = $i->{ "type" };
			$name = $i->{ "name" };

			if ( $type eq "uint8" ) {
				if ( $mode eq "Buf" ) {
					?>
					if ( cur >= len ) {
						BBB_ERR_CODE( BBB_ERROR_SMALLBUFFER, "%" PRIuPTR " bytes", len );
						result = BBB_ERROR_SMALLBUFFER;
						<? c_GotoCleanup(); ?>
					}
					r-><?= $name ?> = *( buf + cur );
					<?
				} else {
					bbb_Call( "?> bbb_util_Fread( &( r-><?= $name ?> ), 1, 1, f, &wasRead ) <?" );
					?>
					if ( wasRead != 1 ) {
						BBB_ERR_CODE( BBB_ERROR_CORRUPTEDDATA );
						result = BBB_ERROR_CORRUPTEDDATA;
						<? c_GotoCleanup(); ?>
					}
					bbb_util_hash_UpdateChecksum( &( r-><?= $name ?> ), 1, chk );
					<?
				}
				?> cur++; <?/
			} else {
				if ( $mode eq "Buf" ) {
					bbb_Call( "?> bbb_bio_ReadFromBuf_<?= $type ?>( &( r-><?= $name ?> ), buf + cur, len - cur ) <?" );
				} else {
					bbb_Call( "?> bbb_bio_ReadFromFile_<?= $type ?>( &( r-><?= $name ?> ), f, chk ) <?" );
				}

				if ( $type eq "varbuf" ) {
					?> cur += bbb_bio_GetSize_varbuf( r-><?= $name ?> ); <?/
					c_OnCleanup( "?>
						if ( BBB_FAILED( result ) ) {
							free( r-><?= $name ?>.buf );
							r-><?= $name ?>.buf = NULL;
							r-><?= $name ?>.len = 0;
						}
					<?" );
				} else {
					?> cur += sizeof( r-><?= $name ?> ); <?/
				}
			}
		}
		?>

		<? c_Cleanup(); ?>
		return result;
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
		bbb_result_t	result = BBB_SUCCESS;
		size_t			cur = 0;
		size_t			i;

		for ( i = 0; i < n; i++ ) { <?/
			if ( $mode eq "Buf" ) {
				bbb_Call( "?> <?= $bio_ns ?>_ReadFrom<?= $mode ?>_<?= $recName ?>( &( a[ i ] ), buf + cur, len - cur ) <?" );
			} else {
				bbb_Call( "?> <?= $bio_ns ?>_ReadFrom<?= $mode ?>_<?= $recName ?>( &( a[ i ] ), f, chk ) <?" );
			}
			?>
			cur += <?= $bio_ns ?>_GetSize_<?= $recName ?>( &( a[ i ] ) );
		}

		<? c_Cleanup(); ?>
		<? if ( $params->{ "isDynamic" } ) { ?>
			if ( BBB_FAILED( result ) ) {
				<?= $bio_ns ?>_DestroyEach_<?= $recName ?>( a, i );			// destroy previously created records
			}
		<? } ?>
		return result;
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
		bbb_result_t	result = BBB_SUCCESS;
		size_t			cur = 0;
		<?
		foreach $i ( @$fields ) {
			my $type = $i->{ "type" };
			my $name = $i->{ "name" };

			if ( $type eq "uint8" ) {
				if ( $mode eq "Buf" ) {
					?>
					if ( cur >= len ) {
						BBB_ERR_CODE( BBB_ERROR_SMALLBUFFER, "%" PRIuPTR " bytes", len );
						result = BBB_ERROR_SMALLBUFFER;
						<? c_GotoCleanup(); ?>
					}
					*( buf + cur ) = r-><?= $name ?>;
					<?
				} else {
					?>
					bbb_util_hash_UpdateChecksum( &( r-><?= $name ?> ), 1, chk );
					<? bbb_Call( "?> bbb_util_Fwrite( &( r-><?= $name ?> ), 1, 1, f ) <?" ); ?>
					<?
				}
				?> cur++; <?/
			} else {
				if ( $mode eq "Buf" ) {
					bbb_Call( "?> bbb_bio_WriteToBuf_<?= $type ?>( r-><?= $name ?>, buf + cur, len - cur ) <?" );
				} else {
					bbb_Call( "?> bbb_bio_WriteToFile_<?= $type ?>( r-><?= $name ?>, f, chk ) <?" );
				}

				if ( $type eq "varbuf" ) {
					?> cur += bbb_bio_GetSize_varbuf( r-><?= $name ?> ); <?/
				} else {
					?> cur += sizeof( r-><?= $name ?> ); <?/
				}
			}
		}
		?>

		<? c_Cleanup(); ?>
		return result;
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
		bbb_result_t	result = BBB_SUCCESS;
		size_t			cur = 0;
		size_t			i;

		for ( i = 0; i < n; i++ ) { <?/
			if ( $mode eq "Buf" ) {
				bbb_Call( "?> <?= $bio_ns ?>_WriteTo<?= $mode ?>_<?= $recName ?>( &( a[ i ] ), buf + cur, len - cur ) <?" );
			} else {
				bbb_Call( "?> <?= $bio_ns ?>_WriteTo<?= $mode ?>_<?= $recName ?>( &( a[ i ] ), f, chk ) <?" );
			}
			?>
			cur += <?= $bio_ns ?>_GetSize_<?= $recName ?>( &( a[ i ] ) );
		}

		<? c_Cleanup(); ?>
		return result;
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

	?> <?= $protos->{ "Copy" } ?> {
		bbb_result_t	result = BBB_SUCCESS;
#ifndef BBB_RELEASE
		int				allocated = 0;				// can become negative
#endif
		<?
		foreach $i ( @$fields ) {
			$type = $i->{ "type" };
			$name = $i->{ "name" };

			if ( IsAtomType $type ) {
				?> dst-><?= $name ?> = src-><?= $name ?>; <?/
			} elsif ( $type eq "varbuf" ) {
				?>
				<? bbb_Call( "?> bbb_util_Malloc( ( void** )&( dst-><?= $name ?>.buf ), src-><?= $name ?>.len ) <?" ); ?>
#ifndef BBB_RELEASE
				allocated++;
#endif
				<? c_OnCleanup( "?>
					if ( BBB_FAILED( result ) ) {
						free( dst-><?= $name ?>.buf );
#ifndef BBB_RELEASE
						allocated--;
#endif
					}
				<?" ); ?>

				memcpy( dst-><?= $name ?>.buf, src-><?= $name ?>.buf, src-><?= $name ?>.len );
				dst-><?= $name ?>.len = src-><?= $name ?>.len;
				<?
			} else {
				die "Unknown field type: $type";
			}
		}
		?>

		<? c_Cleanup(); ?>
#ifndef BBB_RELEASE
		if ( BBB_FAILED( result ) && allocated != 0 ) {
			BBB_ERR_CODE( BBB_ERROR_DEVELOPER, "malloc/free calls inconsistency: %d", allocated );
			result = BBB_ERROR_DEVELOPER;
		}
#endif
		return result;
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
				?> bbb_bio_GetSize_varbuf( r-><?= $name ?> ) <?
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
#include "bbb_util.h"
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
