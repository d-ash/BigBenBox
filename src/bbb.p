<?:include c_lang.p ?>
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
		my $call = shift;

		?>
		if ( BBB_FAILED( result = <?= $call ?> ) ) {
			<? c_GotoCleanup(); ?>
		}
		<?
	}
?>
