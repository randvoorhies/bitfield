#include <iostream>
#include "bitfield.hpp"

int main()
{
  bitfield<8> b;

  b.range<0,3>() = "0101";
  b.range<4,7>() = 0x0f;

  std::cout << "here is the bitfield: " << b.to_string() << " = " << size_t(b.to_num()) << std::endl;

  std::cout << "it's easy to access single elements of a bitfield: " << b[3] << b[2] << b[1] << b[0] << std::endl;

  std::cout << "bitfields can be reversed too: " << b.reversed().to_string() << " = " << size_t(b.reversed().to_num()) << std::endl;

  std::cout << "ranges can be converted to strings or integers: " << b.range<0,3>().to_string() << " = " << b.range<0,3>().to_num() << std::endl;

  // b.range<0,8>();          <- Compile time error! Be sure not to overstep your bitfields! Range values are _inclusive_.
  // b.range<0,1>() = "010";  <- Compile time error! This range can only be assigned two bits.
  // b.range<0,1>() = "12";   <- Runtime error! Only 0's and 1's are allowed in string assignments
  // b.range<0,3>() = 0x10;   <- Runtime error! 4 bits can only hold up to 0x0f.

  return 0;
} 
