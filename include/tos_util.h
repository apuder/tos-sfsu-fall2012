
#ifndef __TOS_UTIL_H__
#define __TOS_UTIL_H__

//#ifndef LINUX
typedef char *va_list;
//#endif

/* Amount of space required in an argument list for an arg of type TYPE.
   TYPE may alternatively be an expression whose type is used.  */

#define __va_rounded_size(TYPE)  \
  (((sizeof (TYPE) + sizeof (int) - 1) / sizeof (int)) * sizeof (int))

#define va_start(AP, LASTARG)                                 \
 (AP = ((char *) &(LASTARG) + __va_rounded_size (LASTARG)))

#define va_end(AP)

#define va_arg(AP, TYPE)                                      \
 (AP += __va_rounded_size (TYPE),                             \
  *((TYPE *) (AP - __va_rounded_size (TYPE))))

#endif
