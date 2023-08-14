aux_source_directory(./ LIBNET)
file(GLOB LIBCONTROL ./control.c)

add_library(net SHARED ${LIBNET})
add_library(control SHARED ${LIBCONTROL})

target_link_libraries(net mqtt log config)