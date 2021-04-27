cmake_minimum_required(VERSION 3.16)

SET(PROJECT_NAME wavToMp3)

project(${PROJECT_NAME})

find_package (Threads)

include_directories(include
                    lame-3.100/include
                    )
                    
link_directories(lame-3.100/libmp3lame/.libs/
                 )

add_executable(${PROJECT_NAME} 
               src/wavToMp3.cpp
               src/main.cpp)
               
target_link_libraries(${PROJECT_NAME} 
                      pthread
                      mp3lame.a)
                             
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
