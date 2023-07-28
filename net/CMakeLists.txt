aux_source_directory(./ LIBNET)

add_library(net SHARED ${LIBNET})

target_link_libraries(net mqtt log)