<?:include bbb_errors.p ?>

<?:prefix @_ bbb_errors_ ?>
<?:prefix @^ BBB_ERRORS_ ?>

#ifndef @^H
#define @^H

#include <stdint.h>

<? for my $i ( 0 .. $#bbb_errors ) { ?>
#define <?= $bbb_errors[ $i ] ?>		<?= $i ?>
<? } ?>


#define BBB_FAILED( call )				( ( call ) != BBB_SUCCESS )

#define BBB_ERR_CODE( code, ... )		do { \
											fprintf( stderr, "%s at %s:%u - ", @_GetText( code ), __FILE__, __LINE__ ); \
											fprintf( stderr, " " __VA_ARGS__ ); \
											fprintf( stderr, "\n" ); \
										} while ( 0 );

typedef uint32_t	bbb_result_t;

const char*			@_GetText( const bbb_result_t code );

#endif
