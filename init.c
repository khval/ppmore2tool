
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define __USE_INLINE__

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/powerpacker.h>
#include <proto/icon.h>


struct PowerPackerIFace	*IPowerPacker = NULL;
struct Library			*PowerPackerBase = NULL;

struct IconIFace		*IIcon = NULL;
struct Library			*IconBase = NULL;

BOOL open_lib( const char *name, int ver , const char *iname, int iver, struct Library **base, struct Interface **interface)
{
	*interface = NULL;
	*base = OpenLibrary( name , ver);
	if (*base)
	{
		 *interface = GetInterface( *base,  iname , iver, TAG_END );
		if (!*interface) printf("Unable to getInterface %s for %s %ld!\n",iname,name,ver);
	}
	else
	{
	   	printf("Unable to open the %s %ld!\n",name,ver);
	}
	return (*interface) ? TRUE : FALSE;
}

BOOL open_libs()
{
	if ( ! open_lib( "powerpacker.library", 0L , "main", 1, &PowerPackerBase, (struct Interface **) &IPowerPacker  ) ) return FALSE;
	if ( ! open_lib( "icon.library", 0L , "main", 1, &IconBase, (struct Interface **) &IIcon  ) ) return FALSE;

	return TRUE;
}

void close_libs()
{
	if (IPowerPacker) DropInterface((struct Interface*) IPowerPacker); IPowerPacker = 0;
	if (PowerPackerBase) CloseLibrary(PowerPackerBase); PowerPackerBase = 0;

	if (IIcon) DropInterface((struct Interface*) IIcon); IIcon = 0;
	if (IconBase) CloseLibrary(IconBase); IconBase = 0;
}

