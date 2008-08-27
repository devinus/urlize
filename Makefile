MODULES = urlize
PG_CPPFLAGS = -arch ppc -arch i386 -arch x86_64
OBJS = urlize.o

PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
