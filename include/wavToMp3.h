/*
 * WaveToMp3.h
 *
 *  Created on: April 10, 2021
 *      Author: vishal gupta
 */
 
#ifndef WAVTOMP3_H
#define WAVTOMP3_H

#include <string>
#include <mutex>
#include <vector>
#include <thread>

namespace wavToMp3Encoding {

struct wavHeader {
 char chunk_id[4];
 int chunk_size;
 char format[4];
 char subchunk1_id[4];
 int subchunk1_size;
 short int audio_format;
 short int num_channels;
 int sample_rate;
 int byte_rate;
 short int block_align;
 short int bits_per_sample;
 char subchunk2_id[4];
 int subchunk2_size;			
};

class wavToMp3 {

 public:
   /*
    * Constructor.
    * dir_path[IN] path of directory which contain wav file.
    */  
   explicit wavToMp3(const std::string& dir_path);
   
   /*
    * Constructor.
    * dir_path[IN] path of directory which contain wav file.
    */  
   void workerFunction(void);

   /*
    * Function to get number of wav file present in a given directory.
    */  
   std::size_t getNumberOfFile() const;
   
   /*
    * This function will create the multiple threads. So parallel
    * file conversion can be done.
    * number_of_thread[IN] represents number of threads you want to create.
    */  
   void startParallelEncoding(const std::size_t number_of_thread = 0);
    
 private:
   std::vector<std::string> file_list_;
   std::mutex m_;
   std::vector<std::thread> thread_pool_;
   const unsigned int kPcmBufferSize_{1024};
   const unsigned int kMp3BufferSize_{8192};

   /*
    * This function will encode the wav to mp3.
    * input_file[IN] input wav file name.
    */  
   bool encodeWavToMp3(const std::string& input_file);
};

} // wavToMp3Encoding
#endif // WAVTOMP3_H
