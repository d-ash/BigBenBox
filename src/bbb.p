<?:include	c_lang.p ?>
<?:macro
	PerlPP::AddPreprocessor( sub {
		my $cont = shift;

		$$cont =~ s/\$cleanup;/${\(TAG_OPEN)} c_Cleanup(); ${\(TAG_CLOSE)}/g;
		$$cont =~ s/\$gotoCleanup;/${\(TAG_OPEN)} c_GotoCleanup(); ${\(TAG_CLOSE)}/g;
		$$cont =~ s/\$call\s+(.+?);/${\(TAG_OPEN)} bbb_Call( "${\(TAG_CLOSE)} $1 ${\(TAG_OPEN)}" ); ${\(TAG_CLOSE)}/gs;
		$$cont =~ s/\$onCleanup(.+?)\$\$/${\(TAG_OPEN)} c_OnCleanup( "${\(TAG_CLOSE)} $1 ${\(TAG_OPEN)}" ); ${\(TAG_CLOSE)}/gs;
	} );
?>
<?
	my @bbb_errors = qw(
		BBB_SUCCESS
		BBB_ERROR_DEVELOPER
		BBB_ERROR_NOMEMORY
		BBB_ERROR_FILESYSTEMIO
		BBB_ERROR_CORRUPTEDDATA
		BBB_ERROR_SMALLBUFFER
	);

	sub bbb_Call {
		my $fnc = shift;

		?>
		if ( BBB_FAILED( result = <?= $fnc ?> ) ) {
			<? c_GotoCleanup(); ?>
		}
		<?
	}
?>
