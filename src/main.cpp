/*
 * main.cpp
 *
 * Created on: April 10, 2021
 * Author    : vishal gupta
 *
 */

#include <iostream> 
#include <filesystem> 
#include <thread>

#include "wavToMp3.h"

int main(int argc, char* argv[]) {
 if (argc != 2) {
   std::cerr<<"Error in execution.\n"
            <<"Execute in format: "<< argv[0]
            <<" <path to wav file directory>"<<std::endl;
   exit(EXIT_FAILURE);
 }
 
 /*
  * First check if the given path is valid.
  */
 if (std::filesystem::exists(argv[1])) {
   wavToMp3Encoding::wavToMp3 wav_to_mp3(argv[1]);

   /* 
    * Check cpu cores and number of wav file to be converted. 
    * So an efficient number of threads can be created for 
    * parallel encoding.
    */
   const std::size_t kNumberOfThread{std::min(wav_to_mp3.getNumberOfFile(),
                                              static_cast<std::size_t>(std::thread::hardware_concurrency()))};

   /*
    * create multiple threads and start the encoding.
    */
   wav_to_mp3.startParallelEncoding(kNumberOfThread);
 } else {
   std::cerr<<"Error invalid path !!!"<<std::endl;
   exit(EXIT_FAILURE);
 }
 return 0;
}
