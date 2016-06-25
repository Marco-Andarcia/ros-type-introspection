
#include "topic_tools/shape_shifter.h"
#include <boost/algorithm/string.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/utility/string_ref.hpp>
#include <geometry_msgs/Pose.h>
#include <sensor_msgs/JointState.h>
#include <tf/tfMessage.h>
#include <sstream>
#include <iostream>
#include <chrono>
#include "ros-type-parser.h"

using namespace ros::message_traits;
using namespace RosTypeParser;

std::vector<SubstitutionRule> Rules()
{
    std::vector<SubstitutionRule> rules;
    rules.push_back( SubstitutionRule(".position[#]",
                                      ".name[#]",
                                      ".#.position") );

    rules.push_back( SubstitutionRule(".velocity[#]",
                                      ".name[#]",
                                      ".#.velocity") );

    rules.push_back( SubstitutionRule(".effort[#]",
                                      ".name[#]",
                                      ".#.effort") );

    rules.push_back( SubstitutionRule(".transforms[#].transform",
                                      ".transforms[#].header.frame_id",
                                      ".transform.#") );

    rules.push_back( SubstitutionRule(".transforms[#].header",
                                      ".transforms[#].header.frame_id",
                                      ".transform.#.header") );
    return rules;
}

int main( int argc, char** argv)
{
    RosTypeParser::RosTypeMap type_map;

    parseRosTypeDescription(
                DataType<tf::tfMessage >::value(),
                Definition<tf::tfMessage>::value(),
                &type_map );

    std::cout << "------------------------------"  << std::endl;
    printRosTypeMap( type_map );

    tf::tfMessage tf_msg;

    tf_msg.transforms.resize(3);

    const char* suffix[3] = { "_A", "_B", "_C" };

    for (int i=0; i< tf_msg.transforms.size() ; i++)
    {
        tf_msg.transforms[i].header.seq = 100+i;
        tf_msg.transforms[i].header.stamp = {1234 + i, 0 };
        tf_msg.transforms[i].header.frame_id = std::string("frame").append(suffix[i]);

        tf_msg.transforms[i].child_frame_id = std::string("child").append(suffix[i]);
        tf_msg.transforms[i].transform.translation.x = 10 +i;
        tf_msg.transforms[i].transform.translation.y = 20 +i;
        tf_msg.transforms[i].transform.translation.z = 30 +i;

        tf_msg.transforms[i].transform.rotation.x = 40 +i;
        tf_msg.transforms[i].transform.rotation.y = 50 +i;
        tf_msg.transforms[i].transform.rotation.z = 60 +i;
        tf_msg.transforms[i].transform.rotation.w = 70 +i;
    }

    RosTypeFlat flat_container;
    std::vector<uint8_t> buffer(64*1024);

    auto start = std::chrono::high_resolution_clock::now();

    for (int i=0; i<10000;i++)
    {
        flat_container.name_id.clear();
        flat_container.value.clear();
        flat_container.value_renamed.clear();

        ros::serialization::OStream stream(buffer.data(), buffer.size());
        ros::serialization::Serializer<tf::tfMessage>::write(stream, tf_msg);

        uint8_t* buffer_ptr = buffer.data();

        buildRosFlatType(type_map, "tfMessage", "msgTransform", &buffer_ptr,  &flat_container);
        applyNameTransform( Rules(), &flat_container );
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = end - start;

    std::cout << "time elapsed: " << std::chrono::duration_cast< std::chrono::milliseconds>( elapsed ).count()  <<   std::endl;

    return 0;
}
