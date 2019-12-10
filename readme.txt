DESCRIPTION

A set of small C89, POSIX (XSI and CX) compliant, base utilities. 

These tools are strickly POSIX conforming I have no interest in adding anything extra
such as -h (human readable) flag, -v (version) flag, etc.
Might add extended utilities such as yes and others but these will have less options
than other implementations.
Some utilities code is very similar to the suckless / plan 9 ones.
The code is refactored with reference to the suckless utilities. There code layout is dece.
Also any places I was unsure of in the POSIX documentation I used suckless and or GNU coreutils
implementation behaviour.


COMPILE

make


INSTALL

make install


MAN

make man


GOAL

To have a set of small base utilities that do not take much resources.
No use of stream functions in the stdandard library.
Written using c89 compliant C code.
No using malloc in code.
Simple error API. No need for stream functions for errors as they are unbuffered.
Install path organizes programs in BSD layout. Everything is where it is supposed
to be (most linux distros throw everything in 2 directories).
These programs are hopefully easy to read and learn. /* Some programs could be changed */

No using library functions that use malloc /* Subject to change */
Using at most 2 BUFSIZ buffers per program /* Subject to change */


FUTURE

Potential future developments:

Would like to have a EMBEDDED flag that avoids usage of any library
functions that use malloc. Also potentially write a similar but different implementation
of nftw that uses the stack and chdir rather than malloc calls and path concatenation
(currently under lib/path.c). The EMBEDDED flag would turn off some POSIX features.
Considering turning of nftw and -R flags for EMBEDDED but it is faster than forking another
process.
These include:
 - du: would print/add multiple occurrences of hardlinked files (because no use of search.h).
 - df: would not have nice aligned output (because no use of search.h).


License

MIT
