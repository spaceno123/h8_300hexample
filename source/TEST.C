/* --- test --- */

#include <stdio.h>

char buf[]={"test ! keyin num<CR>\n"};

void main( void )
{
	int i;

	printf( buf );
	if ( scanf( "%d", &i ) == 1 )
		printf( "%d(=H'%X)\n", i, i );
	else
		printf( " ???\n" );
}
