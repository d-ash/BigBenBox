<?
	my @c_cleanups = ();

	sub c_InitCleanup {
		push( @c_cleanups, [ "", "", 0 ] );
	}

	sub c_OnCleanup {
		my @x = @_;

		push( @x, 0 );
		unshift( @c_cleanups, \@x );
	}

	sub c_GotoCleanup {
		$c_cleanups[ 0 ][ 2 ]++;					# touch this label
		?>goto L_cleanup_<?= $c_cleanups[ 0 ][ 0 ] ?>;<?
	}

	sub c_Cleanup {
		my $x;

		while ( $x = shift( @c_cleanups ) ) {
			if ( $x->[ 2 ] ) {						# show touched labels only
				?>
L_cleanup_<?= $x->[ 0 ] ?>:
				<?
			}
			?>
			<?= $x->[ 1 ] ?>
		<?
		}
		c_InitCleanup();
	}

	c_InitCleanup();
?>
