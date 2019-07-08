PROJECT ( scratchbox2 C )

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)

# Declare select helper functionality...
FUNCTION(PREPEND var prefix)
   SET(listVar "")
   FOREACH(f ${ARGN})
      LIST(APPEND listVar "${prefix}/${f}")
   ENDFOREACH(f)
   SET(${var} "${listVar}" PARENT_SCOPE)
ENDFUNCTION(PREPEND)

# Set basic ground rules for compile...  
SET(LUA_FW "./luafw")
SET(LIBSB2_PATH "./preload")
SET(LUA_PATH "${LUA_FW}/lua-5.1.4/src")
SET(CMAKE_C_FLAGS "-O2 -g -fPIC -Wall -W -Iinclude -I${LUA_PATH} -I${LIBSB2_PATH}")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_GNU_SOURCE=1 -D_LARGEFILE_SOURCE=1 -D_LARGEFILE64_SOURCE=1")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DSCRATCHBOX_ROOT=${PROJECT_SOURCE_DIR}")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wl,--no-as-needed")

# Define the library components we're working with- each one's a component, and should be
# defined down in the project to keep track of things...  But, for now, we'll define it
# up in a global CMakeLists.txt without any autodef magic or subdirs...

# luafw.a
#----------------------------------------------------
LIST(APPEND LUA_SOURCES loadlib.c lstate.c ldebug.c liolib.c llex.c lauxlib.c ldump.c ltablib.c 
	lstrlib.c ldo.c lmathlib.c lapi.c lvm.c ltable.c lgc.c lbaselib.c lopcodes.c linit.c 
	lfunc.c lparser.c lstring.c ldblib.c lzio.c loslib.c ltm.c lundump.c lobject.c lmem.c lcode.c)
PREPEND(LUA_SOURCES ${LUA_PATH} ${LUA_SOURCES})

LIST(APPEND LUA_IF luaif.c sb_log.c paths.c argvenvp.c)
PREPEND(LUA_IF "${LUA_FW}/luaif" ${LUA_IF})

ADD_LIBRARY (luafw STATIC ${LUA_SOURCES} ${LUA_IF})


# libsb2.so ("preload")
#----------------------------------------------------
# Now, let's specify some custom build steps here.  Perl scripting that does this generated code
# so it'll be forced to build this before the library code gets done.
SET(PRELOAD_SOURCES "")
LIST(APPEND GENERATED_CODE master private)
FOREACH(genFile ${GENERATED_CODE})
	ADD_CUSTOM_COMMAND(
		OUTPUT ${LIBSB2_PATH}/${genFile}.c ${LIBSB2_PATH}/${genFile}.h
	 	COMMAND ${LIBSB2_PATH}/gen-interface.pl -W ${LIBSB2_PATH}/${genFile}.c -E ${LIBSB2_PATH}/${genFile}.h < ${LIBSB2_PATH}/interface.${genFile}
	 	DEPENDS ${LIBSB2_PATH}/interface.${genFile}
	 	COMMENT Generating libsb2 ${genFile} wrapper)
	EXECUTE_PROCESS(COMMAND touch ${LIBSB2_PATH}/${genFile}.c ${LIBSB2_PATH}/${genFile}.h)
	LIST(APPEND PRELOAD_SOURCES ${genFile}.c)
ENDFOREACH(genFile ${GENERATED_CODE})

LIST(APPEND PRELOAD_SOURCES libsb2.c sb_exec.c sb_l10n.c glob.c glob64.c 	
	network.c execgates.c miscgates.c tmpnamegates.c fdpathdb.c procfs.c 
	mempcpy.c system.c)
PREPEND(PRELOAD_SOURCES ${LIBSB2_PATH} ${PRELOAD_SOURCES}) 
 
# Specify our library now...
ADD_LIBRARY(sb2 SHARED ${PRELOAD_SOURCES} )
TARGET_LINK_LIBRARIES(sb2 luafw)

# Add the utils binaries to the build now...
ADD_EXECUTABLE(sb2-show ./utils/sb2-show.c)
TARGET_LINK_LIBRARIES(sb2-show sb2 dl m)
ADD_EXECUTABLE(sb2-monitor ./utils/sb2-monitor.c)
TARGET_LINK_LIBRARIES(sb2-monitor sb2 dl m)
ADD_EXECUTABLE(sb2-interp-wrapper ./utils/sb2-interp-wrapper.c)
TARGET_LINK_LIBRARIES(sb2-interp-wrapper sb2 dl m)


# Clean up after ourselves with the pre-defined stuff...
FOREACH(genFile ${GENERATED_CODE})
	EXECUTE_PROCESS(COMMAND rm -f ${LIBSB2_PATH}/${genFile}.*)
ENDFOREACH(genFile ${GENERATED_CODE})

# Add our installs...
INSTALL(TARGETS sb2-show sb2-monitor sb2-interp-wrapper sb2
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib)

# Install a few key scripts and configuration items for this stuff...
INSTALL(PROGRAMS utils/sb2 utils/sb2-init utils/sb2-mkinitramfs utils/sb2-build-libtool utils/sb2-build-qemuserver
              utils/sb2-parse-sb2-init-args utils/sb2-config utils/sb2-qemu-gdbserver-prepare utils/sb2-logz
        DESTINATION bin
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
INSTALL(PROGRAMS utils/sb2-cmp-checkbuilddeps-output.pl
        DESTINATION share/scratchbox2/lib
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
INSTALL(PROGRAMS utils/sb2-upgrade-config utils/sb2-parse-sb2-init-args utils/sb2-config-gcc-toolchain
                 utils/sb2-config-debian utils/sb2-check-pkg-mappings utils/sb2-exitreport utils/sb2-generate-locales
        DESTINATION share/scratchbox2/scripts
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
INSTALL(DIRECTORY lua_scripts
        DESTINATION share/scratchbox2
        DIRECTORY_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
INSTALL(DIRECTORY modeconf
        DESTINATION share/scratchbox2
        DIRECTORY_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
INSTALL(DIRECTORY wrappers
        DESTINATION share/scratchbox2
        DIRECTORY_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
        FILES_MATCHING PATTERN "*"
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
INSTALL(DIRECTORY tests 
        DESTINATION share/scratchbox2
        DIRECTORY_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
        FILES_MATCHING PATTERN "*"
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)


   
