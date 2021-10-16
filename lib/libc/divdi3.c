#include <types.h>
#include <limits.h>

typedef unsigned int UQItype __mode(QI);
typedef          int SItype	 __mode(SI);
typedef unsigned int USItype __mode(SI);
typedef          int DItype  __mode(DI);
typedef unsigned int UDItype __mode(DI);

#define Wtype   SItype
#define HWtype  SItype
#define DWtype  DItype
#define UWtype  USItype
#define UHWtype USItype
#define UDWtype UDItype
#define W_TYPE_SIZE 32