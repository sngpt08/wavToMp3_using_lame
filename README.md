Convert wav to mp3 using the lame library.

This code will scan the dir and prepare the list of wav file present in that dir.
Rest other file present in dir will be ignored. Then it will check the number of
cpu core and the files that needs to be encoded in mp3 format and create the
multiple thread for faster coversion of wav to mp3.

Execute the following cmd:-
1. mkdir build && cd build
2. cmake ..
3. execute "make"
4. ./wavToMp3 "path of dir containing wav file."
