aux_source_directory(./iniparser/ LIBINIPARSER)
aux_source_directory(./zlog/ ZLOG)

add_library(iniparser SHARED ${LIBINIPARSER})
add_library(zlog SHARED ${ZLOG})