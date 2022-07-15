
CFLAGS = -D__USE_INLINE__

objs = init.o

All:	ppmore

ppmore: $(objs) ppmore2tool.c
	gcc  $(CFLAGS) $(objs) ppmore2tool.c -o ppmore
