#include <iostream>
#include "bitfield.hpp"
#include <type_traits>

int main()
{

  // bitfields are created by specifying the number of bits in the field. 
  // Note that the number of bits does not have to be a power of 2, but is must be less than 64 (for now)
  bitfield<8> b1;

  // bitfields can be assigned a number
  b1 = 255;

  // Or, you can use a string to set them
  b1 = "10101010";

  // trying to set a number too large for the bitfield, or a string with the
  // wrong number of characters will result in a compiler error
  // b1 = 256;   // <- Compiler error!
  // b1 = "101"; // <- Compiler error!
  
  // bitfields can be converted to a string, or to an integral type
  std::cout << b1.to_string() << " = " << (int)b1.to_num() << std::endl;

  // Note that the to_num() method returns the smallest integral type that the bitfield could hold
  // e.g. bitfield<16>::to_num() would return a uint16_t, and bitfield<64>::to_num() would return a uint64_t
  std::cout << std::is_same<decltype(b1.to_num()), uint8_t>::value << std::endl;

  // bitfields can be accessed with operator[]
  b1[0] = 1;
  std::cout << b1[3] << b1[2] << b1[1] << b1[0] << std::endl;


  std::cout << b1.range<0,3>().to_string() << std::endl;
  b1.range<4,7>()[0] = 1;

  bitfield<5> b2 = b1.range<0,4>();
  std::cout << b2.to_string() << std::endl;
  std::cout << b2.range<0,1>().reversed().to_string() << std::endl;

  

  //std::cout << << b.to_string() << std::endl;

  //b.range<0,3>() = "0101";
  //b.range<4,7>() = 0x0f;

  //std::cout << "here is the bitfield: " << b.to_string() << " = " << size_t(b.to_num()) << std::endl;

  //std::cout << "it's easy to access single elements of a bitfield: " << b[3] << b[2] << b[1] << b[0] << std::endl;

  //std::cout << "bitfields can be reversed too: " << b.reversed().to_string() << std::endl;

  //std::cout << "ranges can be converted to strings or integers: " << b.range<0,3>().to_string() << " = " << (int)b.range<0,3>().to_num() << std::endl;

  //bitfield<4> b2 = b.range<0,3>();
  //std::cout << "bitfields can be copied from ranges: " << b2.to_string() << std::endl;

  //std::cout << "ranges can also be reversed: " << b.range<0,3>().reversed().to_string() << " = " << (int)b.range<0,3>().reversed().to_num() << std::endl;

  //////b.range<0,8>();          // <- Compile time error! Be sure not to overstep your bitfields! Range values are _inclusive_.
  //////b.range<0,1>() = "010";  // <- Compile time error! This range can only be assigned two bits.
  //////b.range<0,7>() = 256;    // <- Compile time error! The compiler can recognize that this would be a truncation error.
  //////b.range<0,1>() = "12";   // <- Runtime error! Only 0's and 1's are allowed in string assignments
  ////// b.range<0,3>() = 0x10;   // <- Runtime error! 4 bits can only hold up to 0x0f.

  return 0;
} 
