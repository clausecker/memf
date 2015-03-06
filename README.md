The memf Family of Functions
============================

This project contains an experimental set of functions to marshall data
from binary files into in-memory structures in an easy and portable way.
The interface is similar to that of `scanf` and `printf`: Provide a
structure you want to marshall your data into and call a function from
this family with a pointer to such a structure, a data source / drain
and a string describing the fields of the structure and the rest happens
automatically. No long-winded marshalling code is needed.

Example Usage
-------------

In the following example, I want to marshall the content of a FAT12 boot
sector into a useful format. Here's the structure we are going to use.
Notice that it needs to closely mirror the binary representation:

    struct fat_bootsector {
        uint8_t jump_instruction[3];
	uint8_t oem name[8];
	uint16_t bytes_per_sector;
	uint8_t sectors_per_cluster;
	uint16_t reserved_sectors;
	uint8_t fat_copies;
	uint16_t max_dirs;
	uint16_t sector_count;
	uint8_t media_descriptor;
	uint16_t sectors_per_fat;
	uint16_t sectors_per_head;
	uint16_t heads;
	uint32_t hidden_sectors;
	uint32_t sector_count;
    }

Now we can read a binary image of the MBR into this structure:

    mreadf(mbr, "ccc cccccccc hchchhchhhdd", fat_struct);

It's really that simple.

Directives
----------

Supported directives:

    regex   meaning
    (blank) ignored
    c       read / write uint8_t
    h       read / write uint16_t
    d       read / write uint32_t
    l       read / write uint64_t
    i       switch to Intel byte order
    m       switch to Motorola byte order

Future Extensions
-----------------

This is work in progress. A lot of functionality I'd like to have is not
implemented yet. Stay tuned for updates.

    regex   meaning
    a[0-9]+ align to multiple of number
    I       next item in Intel byte order
    M       next item in Motorola byte order
    [0-9]+  next item repeated n times
    (       begin of item group
    )       end of item group
    {       begin of substructure
    }       end of substructure
    '[^']*' match against string / emit string; escape ' as ''
    "[^"]*" ignore strlen bytes / emit string; escape " as ""
    z       match \0 / emit \0
    p       ignore one byte / emit \0
    P       ignore one byte / emit specified byte (use like Px for fill with x)
    s       consume next structure item but do not read / write
    r       rewind structure

Testability
-----------

You might have noticed the absence of a length parameter for the buffer these
functions read from. Such a length paremeter is not required for any usage. The
number of bytes read from or written to the buffer does only depend only on the
content of the structure description string. The function `nopf()` allows you to
compute at runtime how many bytes an invocation of one of the other functions
would read or write. If you want to verify that your formatting string reads the
correct number of bytes, use something like this:

    #define MY_FORMATTING_STRING "ccc cccccccc hchchhchhhdd"
    assert(nopf(MY_FORMATTING_STRING) == 36);
    if (mreadf(mbr, "ccc cccccccc hchchhchhhdd", fat_struct) != 36)
        /* error handling code here */

You might have also noticed the absence of unit tests for this projects. These
will be added in the future.

Portability
-----------

These functions are designed to be portable and self-contained. They should be
suitable for a wide range of both hosted and embedded platforms.

This source code makes use of the library functions `memcpy`, `fread`,
and `fwrite`, the latter two are only required if you plan to use the
functions `freadf` and `fwritef`. The code does not use any C99 language
constructs but assumes the presence of a header `stdint.h` providing the
types `uint8_t`, `uint16_t`, `uint32_t`, and `uint64_t` with the
semantics specified by ISO 9899:1999§7.18.1.1. If your platform does not
provide these types, you can try to make them yourself. That should be
possible unless your platform is really weird. You can remove support
for `uint64_t` by removing the line

    case 'l': process(64); break;

from `memf.c` and all lines containing the number 64 from `fiddle.h`.

This source code makes the following assumptions about the platform it's
running on:

1. The following macro computes the *structure alignment* of a type `t`.
   The meaning of the result of this macro must be for all scalar `t`
   dividable by the minimum alignment requirement for `t`.

        #define structalign(t) offsetof (struct {char; t var;}, var)

2. A structure member of type `t` is placed on the lowest unused (i.e.
   not occupied by a previous structure member) offset in that structure
   such that the offset of `t` from the beginning of the structure is
   dividable by `structalign(t)`.

3. The value of `structalign(t)` where `t` is an array, structure, or
   union type is equal to the maximum of the results of `structalign`
   applied to each member of `t`.

4. For the `freadf` and `fwritef` function only: `uint8_t` is the same
   type as `unsigned char`. If this is not the case, these two functions
   may not be usable but the rest is not affected.

While these assumptions operate outside of what is guaranteed by
ISO 9899 and its successors, they generally hold true on real-world
platforms because specifying structure layout to follow these rules is
a very sensible design decision. This is how all ABIs I know for
byte-addressable machines are designed. Please tell me if the code does
not operate correctly on your machine so I can try to see the way in
which my assumptions are wrong.

Please remember that this code assumes that it is compiled with the same
ABI as the structure it marshalls to / from. If you want to use options
like -fpack-struct or #pragma pack, you can do that but you need to
compile the memf code with the same options as the rest of your code or
it will most likely not work correctly.
