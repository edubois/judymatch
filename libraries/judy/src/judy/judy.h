#ifndef JUDY64_H
#define JUDY64_H
//  Judy arrays 13 DEC 2012 (judy64n.c from http://code.google.com/p/judyarray/ )
//  This code is public domain.

//  Author Karl Malbrain, malbrain AT yahoo.com
//  with assistance from Jan Weiss.
//  modifications (and any bugs) by Mark Pictor, mpictor at gmail

//  Simplified judy arrays for strings and integers
//  Adapted from the ideas of Douglas Baskins of HP.

//  Map a set of keys to corresponding memory cells (uints).
//  Each cell must be set to a non-zero value by the caller.

//  String mappings are denoted by calling judy_open with zero as
//  the second argument.  Integer mappings are denoted by calling
//  judy_open with the Integer depth of the Judy Trie as the second
//  argument.

//  functions:
//  judy_open:  open a new judy array returning a judy object.
//  judy_close: close an open judy array, freeing all memory.
//  judy_clone: clone an open judy array, duplicating the stack.
//  judy_data:  allocate data memory within judy array for external use.
//  judy_cell:  insert a string into the judy array, return cell pointer.
//  judy_strt:  retrieve the cell pointer greater than or equal to given key
//  judy_slot:  retrieve the cell pointer, or return NULL for a given key.
//  judy_key:   retrieve the string value for the most recent judy query.
//  judy_end:   retrieve the cell pointer for the last string in the array.
//  judy_nxt:   retrieve the cell pointer for the next string in the array.
//  judy_prv:   retrieve the cell pointer for the prev string in the array.
//  judy_del:   delete the key and cell for the current stack entry.

#ifdef linux
	#define _FILE_OFFSET_BITS 64
	#define _LARGEFILE_SOURCE
	#define __USE_FILE_OFFSET64

	#include <endian.h>
#else
	#ifdef __BIG_ENDIAN__
		#ifndef BYTE_ORDER
			#define BYTE_ORDER 4321
		#endif
	#else
		#ifndef BYTE_ORDER
			#define BYTE_ORDER 1234
		#endif
	#endif
	#ifndef BIG_ENDIAN
		#define BIG_ENDIAN 4321
	#endif
#endif

typedef unsigned char uchar;
typedef unsigned int uint;
#define PRIuint			"u"

#if defined(__LP64__) || \
	defined(__x86_64__) || \
	defined(__amd64__) || \
	defined(_WIN64) || \
	defined(__sparc64__) || \
	defined(__arch64__) || \
	defined(__powerpc64__) || \
	defined (__s390x__) 
	//	defines for 64 bit
	
	typedef unsigned long long judyvalue;
	typedef unsigned long long JudySlot;
	#define JUDY_key_mask (0x07)
	#define JUDY_key_size 8
	#define JUDY_slot_size 8
	#define JUDY_span_bytes (3 * JUDY_key_size)
	#define JUDY_span_equiv JUDY_2
	#define JUDY_radix_equiv JUDY_8

	#define PRIjudyvalue	"llu"

#else
	//	defines for 32 bit
	
	typedef uint judyvalue;
	typedef uint JudySlot;
	#define JUDY_key_mask (0x03)
	#define JUDY_key_size 4
	#define JUDY_slot_size 4
	#define JUDY_span_bytes (7 * JUDY_key_size)
	#define JUDY_span_equiv JUDY_4
	#define JUDY_radix_equiv JUDY_8

	#define PRIjudyvalue	"u"

#endif

#define JUDY_mask (~(JudySlot)0x07)

//	define the alignment factor for judy nodes and allocations
//	to enable this feature, set to 64

#define JUDY_cache_line 8	// minimum size is 8 bytes

#if defined(STANDALONE) || defined(ASKITIS)
#include <assert.h>
#include <stdio.h>

uint MaxMem = 0;

// void judy_abort (char *msg) __attribute__ ((noreturn)); // Tell static analyser that this function will not return
void judy_abort (char *msg)
{
	fprintf(stderr, "%s\n", msg);
	exit(1);
}
#endif

#define JUDY_seg	65536

enum JUDY_types {
	JUDY_radix		= 0,	// inner and outer radix fan-out
	JUDY_1			= 1,	// linear list nodes of designated count
	JUDY_2			= 2,
	JUDY_4			= 3,
	JUDY_8			= 4,
	JUDY_16			= 5,
	JUDY_32			= 6,
#ifdef ASKITIS
	JUDY_64			= 7
#else
	JUDY_span		= 7 	// up to 28 tail bytes of key contiguously stored
#endif
};

static int JudySize[] = {
	(JUDY_slot_size * 16),						// JUDY_radix node size
	(JUDY_slot_size + JUDY_key_size),			// JUDY_1 node size
	(2 * JUDY_slot_size + 2 * JUDY_key_size),
	(4 * JUDY_slot_size + 4 * JUDY_key_size),
	(8 * JUDY_slot_size + 8 * JUDY_key_size),
	(16 * JUDY_slot_size + 16 * JUDY_key_size),
	(32 * JUDY_slot_size + 32 * JUDY_key_size),
#ifndef ASKITIS
	(JUDY_span_bytes + JUDY_slot_size)
#else
	(64 * JUDY_slot_size + 64 * JUDY_key_size)
#endif
};

static judyvalue JudyMask[9] = {
0, 0xff, 0xffff, 0xffffff, 0xffffffff,
#if JUDY_key_size > 4
0xffffffffffULL, 0xffffffffffffULL, 0xffffffffffffffULL, 0xffffffffffffffffULL
#endif
};

typedef struct {
	void *seg;			// next used allocator
	uint next;			// next available offset
} JudySeg;

typedef struct {
	JudySlot next;		// judy object
	uint off;			// offset within key
	int slot;			// slot within object
} JudyStack;

typedef struct {
	JudySlot root[1];	// root of judy array
	void **reuse[8];	// reuse judy blocks
	JudySeg *seg;		// current judy allocator
	uint level;			// current height of stack
	uint max;			// max height of stack
	uint depth;			// number of Integers in a key, or zero for string keys
	JudyStack stack[1];	// current cursor
} Judy;

#ifdef ASKITIS

#if JUDY_key_size < 8
#define JUDY_max	JUDY_16
#else
#define JUDY_max	JUDY_64
#endif
#else
#define JUDY_max	JUDY_32
#endif

#ifdef __cplusplus
extern "C" {
#endif

    /// open a new judy array returning a judy object.
    Judy * judy_open( uint max, uint depth );

    /// close an open judy array, freeing all memory.
    void judy_close( Judy * judy );

    /// clone an open judy array, duplicating the stack.
    Judy * judy_clone( Judy * judy );

    /// allocate data memory within judy array for external use.
    void * judy_data( Judy * judy, uint amt );

    /// insert a key into the judy array, return cell pointer.
    JudySlot * judy_cell( Judy * judy, const uchar * buff, uint max );

    /// retrieve the cell pointer greater than or equal to given key
    JudySlot * judy_strt( Judy * judy, const uchar * buff, uint max );

    /// retrieve the cell pointer, or return NULL for a given key.
    JudySlot * judy_slot( Judy * judy, const uchar * buff, uint max );

    /// retrieve the string value for the most recent judy query.
    unsigned int judy_key( Judy * judy, uchar * buff, uint max );

    /// retrieve the cell pointer for the last string in the array.
    JudySlot * judy_end( Judy * judy );

    /// retrieve the cell pointer for the next string in the array.
    JudySlot * judy_nxt( Judy * judy );

    /// retrieve the cell pointer for the prev string in the array.
    JudySlot * judy_prv( Judy * judy );

    /// delete the key and cell for the current stack entry.
    JudySlot * judy_del( Judy * judy );

#ifdef __cplusplus
}
#endif

#endif //JUDY64_H
