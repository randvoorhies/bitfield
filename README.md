# bitfield #
#### A C++ class for convenient bit twiddling ####

    bitfield<16> b;
    
    b.range<0,3>() = "0101"; // Set some bits with a character string
    b.range<4,7>() = 0x0f;   // Set some bits with an integer
    
    std::cout << "bitfield: " << b.to_string() << " = " << b.to_num() << std::endl;

