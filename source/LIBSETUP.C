/* ADJ-702-137C (HITACH C compiler user's manual) p124 */

#include <stdlib.h>

const size_t _sbrk_size = 516;
extern void _INIT_LOWLEVEL(void);
void _INIT_IOLIB(void);
void _INIT_OTHERLIB(void);

void _INITLIB(void)
{
	errno = 0;

	_INIT_LOWLEVEL();
	_INIT_IOLIB();
	_INIT_OTHERLIB();
}

/* ADJ-702-137C (HITACH C compiler user's manual) p125 */

#define _NFILES 4

#include <stdio.h>

const int _nfiles = _NFILES;

struct {
	unsigned char *_bufptr;
	long _bufcnt;
	unsigned char *_bufbase;
	long _buflen;
	char _ioflag1;
	char _ioflag2;
	char _iofd;
} iob[_NFILES];

void _INIT_IOLIB( void )
{
	FILE *fp;

	for ( fp=_iob; fp<_iob+_nfiles; fp++) {
		fp->_bufptr = NULL;
		fp->_bufcnt = 0;
		fp->_buflen = 0;
		fp->_bufbase = NULL;
		fp->_ioflag1 = 0;
		fp->_ioflag2 = 0;
		fp->_iofd = 0;
	}
	if ( freopen("stdin", "r", stdin) == NULL )
		stdin->_ioflag1 = 0xff;
	stdin->_ioflag1 |= _IOUNBUF;
	if ( freopen("stdout", "w", stdout) == NULL )
		stdout->_ioflag1 = 0xff;
	stdout->_ioflag1 |= _IOUNBUF;
	if ( freopen("stderr","w",stderr) == NULL )
		stderr->_ioflag1 = 0xff;
	stderr->_ioflag1 |= _IOUNBUF;
}

/* ADJ-702-137C (HITACH C compiler user's manual) p126 */

#include <stddef.h>

extern char *_s1ptr;
extern void srand(unsigned int);

void _INIT_OTHERLIB( void )
{
	srand(1);
	_s1ptr=NULL;
}

/* ADJ-702-137C (HITACH C compiler user's manual) p127 */

void _CLOSEALL( void )
{
	int i;

	for ( i=0; i<_nfiles; i++ )
		if ( _iob[i]._ioflag1 & ( _IOREAD | _IOWRITE | _IORW ) )
			fclose( &_iob[i] );
}

/* ADJ-702-137C (HITACH C compiler user's manual) p235..p238 */

#include <string.h>

#define STDIN 0
#define STDOUT 1
#define STDERR 2

#define FLMIN 0
#define FLMAX 3

#define O_RDONLY 0x0001
#define O_WRONLY 0x0002
#define O_RDWR 0x0004

#define CR 0x0d
#define LF 0x0a

#define HEAPSIZE 2064

extern void charput( char );
extern char charget( void );

char flmod[FLMAX];

static union {
	short dummy;
	char heap[HEAPSIZE];
} heap_area;

static char *brk=(char *)&heap_area;

int open( char *name, int mode, int flg )
{
	if ( strcmp(name, "stdin") == 0 ) {
		if ( (mode & O_RDONLY) == 0 )
			return -1;
		flmod[STDIN] = mode;
		return STDIN;
	} else if ( strcmp(name, "stdout") == 0 ) {
		if ( (mode & O_WRONLY) == 0 )
			return -1;
		flmod[STDOUT] = mode;
		return STDOUT;
	} else if ( strcmp(name, "stderr") == 0 ) {
		if ( (mode & O_WRONLY) == 0 )
			return -1;
		flmod[STDERR] = mode;
		return STDERR;
	} else
		return -1;
}

int close( int fileno )
{
	if ( (fileno < FLMIN) || (FLMAX <= fileno) )
		return -1;
	flmod[fileno] = 0;
	return 0;
}

int read( int fileno, char *buf, int count )
{
	int i;

	if ( (flmod[fileno] & O_RDONLY) || (flmod[fileno] & O_RDWR) ) {
		for ( i = count; i > 0; i-- ) {
			*buf = charget();
			if ( *buf == CR ) *buf = LF;	/* for console ? */
			buf++;
		}
		return count;
	} else
		return -1;
}

int write( int fileno, char *buf, int count )
{
	int i;
	char c;

	if ( (flmod[fileno] & O_WRONLY) || (flmod[fileno] & O_RDWR) ) {
		for ( i = count; i > 0; i-- ) {
			c = *buf++;
			if ( c == LF ) charput( CR );	/* for console */
			charput( c );
		}
		return count;
	} else
		return -1;
}

long lseek( int fileno, long offset, int base )
{
	return -1l;
}

char *sbrk( int size )
{
	char *p;

	if ( brk+size>heap_area.heap+HEAPSIZE )
		return (char *)-1;
	p = brk;
	brk += size;
	return p;
}
