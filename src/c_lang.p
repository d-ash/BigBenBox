<?
	my @c_cleanups = ();

	sub c_OnCleanup {
		my @x = @_;

		push( @x, 0 );
		push( @c_cleanups, \@x );
	}

	sub c_InitCleanup {
		c_OnCleanup( "" );
	}

	sub c_GotoCleanup {
		$c_cleanups[ $#c_cleanups ][ 1 ]++;			# touch this label
		?>goto L_cleanup_<?= $#c_cleanups ?>;<?
	}

	sub c_Cleanup {
		my $x;
		my $i;

		for ( $i = $#c_cleanups; $i >= 0; $i-- ) {
			$x = pop( @c_cleanups );
			if ( $x->[ 1 ] ) {						# show touched labels only
				?>
L_cleanup_<?= $i ?>:
				<?
			}
			?>
			<?= $x->[ 0 ] ?>
		<?
		}
		c_InitCleanup();
	}

	c_InitCleanup();
?>
