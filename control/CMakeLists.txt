aux_source_directory(./file/ LIBFILEEXTEND)

add_library(fileextend SHARED ${LIBFILEEXTEND})

target_link_libraries(fileextend control)