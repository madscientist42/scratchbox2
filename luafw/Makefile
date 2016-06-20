LUASRC = luafw/lua-5.1.4/src

lua_sources := loadlib.c lstate.c ldebug.c liolib.c llex.c lauxlib.c ldump.c \
	ltablib.c lstrlib.c ldo.c lmathlib.c lapi.c lvm.c ltable.c lgc.c \
	lbaselib.c lopcodes.c linit.c lfunc.c lparser.c lstring.c ldblib.c \
	lzio.c loslib.c ltm.c lundump.c lobject.c lmem.c lcode.c

lua_sources_expanded = $(foreach v,$(lua_sources),$(LUASRC)/$(v))

lua_objs := $(lua_sources_expanded:.c=.o)

objs := $(D)/luaif/luaif.o $(D)/luaif/sb_log.o $(D)/luaif/paths.o $(D)/luaif/argvenvp.o

$(D)/sb_log.o: preload/exported.h include/scratchbox2_version.h

luafw/libluafw.a: $(objs) $(lua_objs)
luafw/libluafw.a: override CFLAGS := $(CFLAGS) -O2 -g -fPIC -Wall -W -I$(SRCDIR)/$(LUASRC) -I$(OBJDIR)/preload -I$(SRCDIR)/preload
luafw/libluafw.a: override LDFLAGS := $(LDFLAGS)
luafw/libluafw.a: override LIBS := 

targets := $(targets) $(D)/libluafw.a

