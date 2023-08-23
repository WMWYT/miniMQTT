aux_source_directory(./file/ LIBFILEEXTEND)
aux_source_directory(./json/ LIBJSONEXTEND)

add_library(fileextend SHARED ${LIBFILEEXTEND})
add_library(jsonextend SHARED ${LIBJSONEXTEND})

target_link_libraries(fileextend control)
target_link_libraries(jsonextend control)