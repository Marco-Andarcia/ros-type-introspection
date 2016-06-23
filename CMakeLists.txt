include_directories( ./ ../  ../../common )

SET( ROS_DIRECTORY /opt/ros/indigo)


include_directories(
    include
     ${ROS_DIRECTORY}/include )

SET( SRC_RTI
   include/ros-type-parser.h
   src/ros-type-parser.cpp

)

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 "  )


add_library(RosTypeIntrospection ${SRC_RTI} )



INSTALL(TARGETS RosTypeIntrospection DESTINATION ${CMAKE_INSTALL_PREFIX} )

add_executable( parser_test   src/parser_test.cpp)

target_link_libraries( parser_test
    RosTypeIntrospection
    ${ROS_DIRECTORY}/lib/libroscpp.so
    ${ROS_DIRECTORY}/lib/librosbag.so
    ${ROS_DIRECTORY}/lib/librosbag_storage.so
    ${ROS_DIRECTORY}/lib/librostime.so
    ${ROS_DIRECTORY}/lib/libroscpp_serialization.so
    )