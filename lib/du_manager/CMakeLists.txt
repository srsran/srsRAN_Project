
set(SOURCES du_manager_factory.cc du_manager_impl.cc)

add_library(srsgnb_du_manager STATIC ${SOURCES})
target_link_libraries(srsgnb_du_manager)
