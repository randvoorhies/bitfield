#pragma once

#include <array>
#include <type_traits>
#include <stdexcept>
#include <string>
#include <algorithm>

template<size_t bits>
class bitfield
{
  private:

    //! A helper class to determine which integer type can hold all of our bits
    /*! Note that this limits us to 64 bit bitfields! */
    template<size_t size, class enable=void> struct uint_x_t { typedef void type; };
    template<size_t size> struct uint_x_t<size, typename std::enable_if<(size <= 8)>::type >               { typedef uint8_t  type; };
    template<size_t size> struct uint_x_t<size, typename std::enable_if<(size > 8 && size <= 16)>::type>   { typedef uint16_t type; };
    template<size_t size> struct uint_x_t<size, typename std::enable_if<(size > 16 && size <= 32)>::type>  { typedef uint32_t type; };
    template<size_t size> struct uint_x_t<size, typename std::enable_if<(size > 32 && size <= 64)>::type>  { typedef uint64_t type; };
    static_assert(bits <= 64, "bitfield must be created with <= 64 bits");

    typedef std::array<bool, bits> storage_t;

    //! A range class holds a reference to the parent bitfield, and can be used to set a range of its bits
    template<size_t b, size_t e>
    struct Range
    {
      Range(bitfield<bits> & parent) : parent_(parent)
      { }

      //! Assign a character string to the range, e.g. mybitset.range<2,4>() = "101";
      template<std::size_t N>
        void operator=(char const (& x) [N] ) 
        {
          static_assert(N-1 == e-b+1, "Wrong number of characters in range assignment");
          for(size_t i=b; i<=e; ++i) 
          {
            if(x[i-b] == '0' || x[i-b] == '1') 
              parent_[e-i] = (x[i-b] == '1');
            else 
              throw std::invalid_argument("Only 0 and 1 are allowed in assignment strings. You gave " + std::string(1, x[b-i]));
          }
        }

      //! Assign an integer value to the range, e.g. mybitset.range<0,7>() = 0xFA;
      template<class T>
        void operator=(T v)
        {
          static_assert(std::is_integral<T>::value, "bitset::range can only be assigned integer values or character literals");
          if(v > ((1 << (e-b+1)) - 1))
            throw std::invalid_argument("Too large a value given to range");

          for(size_t i=b; i<=e; ++i)
          {
            parent_[i] = v & 0x01;
            v = v >> 1;
          }
        }

      //! Convert the bitfield range to a string for printing
      std::string to_string()
      {
        std::string s(e-b+1, '-');
        for(size_t i=b; i<=e; ++i)
          s[i] = parent_[e-i] ? '1' : '0';
        return s;
      }

      //! Convert the bitfield to a number
      typename uint_x_t<e-b+1>::type to_num()
      {
        native_type n(0);
        for(size_t i=b; i<=e; ++i)
          if(parent_[i]) n += (0x01 << i);

        return n;
      }

      bitfield<bits> & parent_;
    };

  public:

    //! The integral type that this bitfield can be converted to. See to_num() for details
    typedef typename uint_x_t<bits>::type native_type;

    //! Default constructor - set to all zeros
    bitfield()
    {
      b_.fill(false);
    }

    //! Copy constructor
    bitfield(bitfield<bits> const & other) : b_(other.b_) 
  { }

    //! Access a range of the bitfield
    template<size_t b, size_t e>
      Range<b,e> range() 
      { 
        static_assert(b <= e,   "bitfield<bits>::range<b,e> must be called with b <= e");
        static_assert(e < bits, "bitfield<bits>::range<b,e> must be called with b and e < bits");

        return Range<b,e>(*this);
      }

    //! Convert the bitfield to a string for printing
    std::string to_string()
    {
      return this->range<0,bits-1>().to_string();
    }

    //! Convert the bitfield to a number
    typename uint_x_t<bits>::type to_num()
    {
      return this->range<0,bits-1>().to_num();
    }

    //! Access a single bit of the bitfield
    bool & operator[](size_t i)
    {
      return b_[i];
    }

    //! Reverse the bitfield in place
    void reverse()
    {
      std::reverse(b_.begin(), b_.end());
    }

    //! Reverse a copy of the bitfield and return it
    bitfield<bits> reversed()
    {
      bitfield<bits> other(*this);
      other.reverse();
      return other;
    }

  private:
    storage_t b_;
};

