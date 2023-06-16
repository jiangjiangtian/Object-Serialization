## Overview
The project is about serializing and deserializing objects, including arithmetic_types, std::string and some STL containers(std::pair, std::tuple, std::map, std::set, std::list, std::vector, std::unique_ptr and std::shared_ptr). By the way, we also support the serialization and deserialization of user-defined objects. To serialize and deserialize user-defined objects, you should first define function get_all_member, of which the return type is std::tuple<...>, and a constructor to construct a object for every member variables.(for details, you can see the test file)
We provide two mechanisms to serialize and deserialize objects, binary and xml. Detaild uses are shown in the test file

## files
include/
    - helper.h: the type traits classes and tuple helper classes and functions
    - binary.h: the interfaces about binary serialization and deserialization
    - xml.h: a wrapper module of tinyxml2 to support XML serialization
    - tinyxml2.h: a C++ XML parser (see https://github.com/leethomason/tinyxml2)

src/
    - tinyxml2.cpp: the implementation of tinyxml2.h
    - test_binary.cpp: the test file of binary serialization and deserialization
    - test_xml.cpp: the test file of xml serialization and deserialization