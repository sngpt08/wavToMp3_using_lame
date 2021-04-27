/*
 * WaveToMp3.cpp
 *
 *  Created on: April 10, 2021
 *      Author: vishal gupta
 */

#include "wavToMp3.h"

#include <iostream>
#include <filesystem>
#include <fstream>
#include <cstring>

#include "lame.h"

namespace wavToMp3Encoding {

wavToMp3::wavToMp3(const std::string& dir_path) {
 for (auto& p: std::filesystem::recursive_directory_iterator(dir_path)) {
   std::filesystem::path file_(p);
   if (file_.extension() == ".wav") {
     file_list_.push_back(file_.string());
   }
 }
}

size_t wavToMp3::getNumberOfFile() const {
 return file_list_.size();
}

void wavToMp3::workerFunction(void) {  
 std::string wav_file;
 while(1) {
   {
     const std::lock_guard<std::mutex> lock(m_);
     if (file_list_.empty()) {
       break;
     }
     wav_file = file_list_.back();
     file_list_.pop_back();
   }
   if (!encodeWavToMp3(wav_file)) {
     std::cerr<<"Error in coding file: "<<wav_file<<std::endl;
   }
 }
}

bool wavToMp3::encodeWavToMp3(const std::string& input_file) {
 std::ifstream wav;
 std::ofstream mp3;
 std::string output_file(input_file);
 output_file.replace(output_file.end()-3,output_file.end(),"mp3");
 try {
   wav.open(input_file, std::ios_base::binary);
   mp3.open(output_file,std::ios_base::binary);
 } catch(std::ifstream::failure& e) {
   std::cerr<<"Error in open of input/output file"<<std::endl;
   return false;
 }
 std::cout<<"started converting " << input_file <<std::endl;
 lame_global_flags* gf = lame_init();
 if (gf == NULL) {
   wav.close();
   mp3.close();
   std::cerr<< "Error in lame_init."<<std::endl;
   return false;
 }

 char wav_header[sizeof(struct wavHeader)];
 wav.read(wav_header,sizeof(struct wavHeader));
 wavHeader wav_metadata;
 memcpy (static_cast<void*>(&wav_metadata),wav_header,sizeof(struct wavHeader));
 
 if (wav_metadata.bits_per_sample != 16) {
   std::cerr<<"Supported bits per sample is 16. "<< wav_metadata.bits_per_sample << " bit is not supported."<<std::endl;
   wav.close();
   mp3.close();
   std::filesystem::remove(output_file);
   return false;
 }

 lame_set_mode(gf, (wav_metadata.num_channels == 1) ? MPEG_mode::MONO : MPEG_mode::STEREO);
 lame_set_quality(gf,0);
 lame_set_num_channels(gf,wav_metadata.num_channels);
 lame_set_in_samplerate(gf,wav_metadata.sample_rate);

 bool retVal = lame_init_params(gf);
 if (retVal < 0) {
   std::cerr<< "Error in lame_init_params."<<std::endl;
   return false;
 }

 unsigned char mp3Buffer[kMp3BufferSize_];
 short int intputData[kPcmBufferSize_];
 int out_write;
 while (wav.good()) {
   wav.read(reinterpret_cast<char*>(intputData),kPcmBufferSize_);
   int read = wav.gcount() / (sizeof(wav_metadata.bits_per_sample) * wav_metadata.num_channels);
   if (wav_metadata.num_channels == 1) {
     out_write = lame_encode_buffer(gf,intputData,NULL,read,mp3Buffer,kMp3BufferSize_);
   } else {
     out_write = lame_encode_buffer_interleaved(gf,intputData,read,mp3Buffer,kMp3BufferSize_);
   }
   if (out_write > 0) {
     mp3.write(reinterpret_cast<char*>(mp3Buffer),out_write);
   }
 }

 wav.close();
 mp3.close();
 lame_close(gf);
 std::cout<<"Done. "<<output_file<<std::endl;
 return true;
}

void wavToMp3::startParallelEncoding(const std::size_t number_of_thread) {
 std::cout<<"creating "<<number_of_thread<< " parallel thread."<<std::endl;
 for (std::size_t i = 0;i<number_of_thread; i++) {
   std::thread t = std::thread(&wavToMp3::workerFunction,this);
   thread_pool_.push_back(std::move(t));
 }

 for (auto& tj : thread_pool_) {
   tj.join();
 }
}

} // wavToMp3Encoding
