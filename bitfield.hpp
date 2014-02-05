#pragma once

#include <array>
#include <type_traits>
#include <stdexcept>
#include <string>
#include <algorithm>
#include <iostream>

template<size_t n_bits>
class bitfield
{
  private:

    //! A helper class to determine which integer type can hold all of our bits
    /*! Note that this limits us to 64 bit bitfields! */
    template<size_t size, class enable=void> struct uintx_t { typedef void type; };
    template<size_t size> struct uintx_t<size, typename std::enable_if<(size <= 8)>::type >               { typedef uint8_t  type; };
    template<size_t size> struct uintx_t<size, typename std::enable_if<(size > 8 && size <= 16)>::type>   { typedef uint16_t type; };
    template<size_t size> struct uintx_t<size, typename std::enable_if<(size > 16 && size <= 32)>::type>  { typedef uint32_t type; };
    template<size_t size> struct uintx_t<size, typename std::enable_if<(size > 32 && size <= 64)>::type>  { typedef uint64_t type; };
    static_assert(n_bits <= 64, "bitfield must be created with <= 64 bits");

  public:

    //! A range class holds a reference to the parent bitfield, and can be used to set a range of its bits
    template<size_t b, size_t e>
      struct Range
      {
        static size_t const n_range_bits = e-b+1;

        //! The integral type that this range can store.
        typedef typename uintx_t<n_range_bits>::type native_range_type;

        Range(bitfield<n_bits> & parent) : parent_(parent)
        { }

        //! Assign a character string to the range, e.g. mybitset.range<2,4>() = "101";
        template<std::size_t N>
          void operator=(char const (& x) [N] ) 
          {
            static_assert(N-1 == n_range_bits, "Wrong number of characters in range assignment");
            for(size_t i=b; i<=e; ++i) 
            {
              if(x[i-b] == '0' || x[i-b] == '1') 
                parent_[e-i] = (x[i-b] == '1');
              else 
                throw std::invalid_argument("Only 0 and 1 are allowed in assignment strings. You gave " + std::string(1, x[b-i]));
            }
          }

        //! Assign an integer value to the range, e.g. mybitset.range<0,7>() = 0xFA;
        void operator=(native_range_type v)
        {
          if(v > ((1 << n_range_bits) - 1))
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
          std::string s(n_range_bits, '-');
          for(size_t i=0; i<n_range_bits; ++i)
            s[n_range_bits-i-1] = (*this)[i] ? '1' : '0';
          return s;
        }

        //! Convert the bitfield to a number
        typename uintx_t<n_range_bits>::type to_num()
        {
          native_type n(0);
          for(size_t i=0; i<n_range_bits; ++i)
            if((*this)[i]) n += (0x01 << i);

          return n;
        }

        bool & operator[](size_t i)
        {
          if(reversed_)
            return parent_.b_[e-i];
          else
            return parent_.b_[b+i];
        }

        //! Reverse the bitfield in place
        /*! This will actually reverse the bits in the original bitfield*/
        void reverse()
        {
          std::reverse(&(*this)[0], &(*this)[n_range_bits-1]);
        }

        //! Return a "view" of the bitfield range with the bits reversed
        /*! This is a non-destructive call, and will not actually reverse any bits in the parent bitfield */
        Range<b,e> reversed()
        {
          Range<b,e> other = *this;
          other.reversed_ = !reversed_;
          return other;
        }

        bitfield<n_bits> & parent_;
        bool reversed_ = false;
      };


    //! The native storage type
    typedef std::array<bool, n_bits> storage_t;

    //! The integral type that this bitfield can store.
    typedef typename uintx_t<n_bits>::type native_type;

    //! Default constructor - set to all zeros
    bitfield()
    {
      b_.fill(false);
    }

    //! Construct from an integer value
    bitfield(native_type v)
    {
      range<0,n_bits-1>() = v;
    }

    //! Copy constructor
    bitfield(bitfield<n_bits> const & other) : b_(other.b_) { }

    //! Access a range of the bitfield
    template<size_t b, size_t e>
      Range<b,e> range() 
      { 
        static_assert(b <= e,   "bitfield<bits>::range<b,e> must be called with b <= e");
        static_assert(e < n_bits, "bitfield<bits>::range<b,e> must be called with b and e < bits");

        return Range<b,e>(*this);
      }

    //! Assign a character string to the bitfield, e.g. bitset<3> mybitset; mybitset = "101";
    template<std::size_t N>
      void operator=(char const (& x) [N] ) 
      {
        range<0,n_bits-1>() = x;
      }

    //! Assign an integer value to the bitfield, e.g. bitset<8> mybitset; mybitset = 0xFA;
    void operator=(native_type v)
    {
      range<0,n_bits-1>() = v;
    }

    //! Convert the bitfield to a string for printing
    std::string to_string()
    {
      return this->range<0,n_bits-1>().to_string();
    }

    //! Convert the bitfield to a number
    typename uintx_t<n_bits>::type to_num()
    {
      return this->range<0,n_bits-1>().to_num();
    }

    //! Access a single bit of the bitfield
    bool & operator[](size_t i)
    {
      return this->range<0,n_bits-1>()[i];
    }

    //! Reverse the bitfield in place
    /*! \todo This needs to just self-assign the range reversed */
    void reverse()
    {
      std::reverse(b_.begin(), b_.end());
    }

    //! Reverse a copy of the bitfield and return it
    /*! \todo This needs to just return the range reversed */
    bitfield<n_bits> reversed()
    {
      bitfield<n_bits> other(*this);
      other.reverse();
      return other;
    }

  private:
    storage_t b_;
};

