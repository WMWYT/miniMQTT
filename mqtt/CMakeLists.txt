aux_source_directory(./ LIBMQTT)

add_library(mqtt SHARED ${LIBMQTT})

target_link_libraries(mqtt log)