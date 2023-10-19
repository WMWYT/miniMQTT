aux_source_directory(./file/ LIBFILEEXTEND)
aux_source_directory(./json/ LIBJSONEXTEND)
aux_source_directory(./test/ LIBTESTEXTEND)

add_library(fileextend SHARED ${LIBFILEEXTEND})
add_library(jsonextend SHARED ${LIBJSONEXTEND})
add_library(testextend SHARED ${LIBTESTEXTEND})

target_link_libraries(fileextend control)
target_link_libraries(jsonextend control)
target_link_libraries(testextend control)