# steganography
Hide any file in .bmp file

Usage: steganography [MODE] [OPTIONS]... 
       steganography -e -i INPUT_BMP -o OUTPUT_BMP -s SECRET_FILE
       steganography -d -i INPUT_BMP -o OUTPUT_SECRET 

Hide any file into BMP file. 

MODES: 
  -d             decode 
  -e             encode 

OPTIONS: 
  -o             output file 
  -i             input file 

encoding only: 
  -s             secret file to encode

custom number of bits to use for encoding: 
  -r             bits of red 
  -g             bits of green  
  -b             bits of blue 
  -a             bits of alpha (32 bit only) 
default values when none provided are (0, 2, 2, 2) -> (A, R, G, B) 


Tymoteusz Dolega 2020 
