<?:include bbb.p ?>
<?:prefix @_ bbb_errors_ ?>
<?:prefix @^ BBB_ERRORS_ ?>

#include "bbb_errors.h"

static char* @_strings[] = {
	<? for my $i ( 0 .. $#bbb_errors ) { ?>
		<?= PerlPP::DQuoteString( $bbb_errors[ $i ] ) ?><? if ( $i < $#bbb_errors ) { ?>,<? } ?>
	<? } ?>
};

const char* @_GetText( const bbb_result_t code ) {
	return @_strings[ code ];
}
