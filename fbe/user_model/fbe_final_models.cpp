// Automatically generated by the Fast Binary Encoding compiler, do not modify!
// https://github.com/chronoxor/FastBinaryEncoding
// Source: FBE
// Version: 1.7.0.0

#include "fbe_final_models.h"

namespace FBE {

uint64_t FinalModel<decimal_t>::extract(double a) noexcept
{
    uint64_t result;
    std::memcpy(&result, &a, sizeof(double));
    return result;
}

uint64_t FinalModel<decimal_t>::uint32x32(uint32_t a, uint32_t b) noexcept
{
    return (uint64_t)a * (uint64_t)b;
}

void FinalModel<decimal_t>::uint64x64(uint64_t a, uint64_t b, uint64_t& low64, uint32_t& high32) noexcept
{
    uint64_t low = uint32x32((uint32_t)a, (uint32_t)b);
    uint64_t mid = uint32x32((uint32_t)a, (uint32_t)(b >> 32));
    uint64_t high = uint32x32((uint32_t)(a >> 32), (uint32_t)(b >> 32));
    high += (mid >> 32);
    low += (mid <<= 32);
    // Test for carry
    if (low < mid)
        high++;

    mid = uint32x32((uint32_t)(a >> 32), (uint32_t)b);
    high += (mid >> 32);
    low += (mid <<= 32);
    // Test for carry
    if (low < mid)
        high++;

    if (high > 0xFFFFFFFFu)
    {
        low64 = 0;
        high32 = 0;
    }
    low64 = low;
    high32 = (uint32_t)high;
}

size_t FinalModel<decimal_t>::verify() const noexcept
{
    if ((_buffer.offset() + fbe_offset() + fbe_size()) > _buffer.size())
        return std::numeric_limits<std::size_t>::max();

    return fbe_size();
}

size_t FinalModel<decimal_t>::get(decimal_t& value) const noexcept
{
    if ((_buffer.offset() + fbe_offset() + fbe_size()) > _buffer.size())
        return 0;

    // Value taken via reverse engineering the double that corresponds to 2^64
    const double ds2to64 = 1.8446744073709552e+019;

    // Read decimal parts
    uint64_t low = *((const uint64_t*)(_buffer.data() + _buffer.offset() + fbe_offset()));
    uint32_t high = *((const uint32_t*)(_buffer.data() + _buffer.offset() + fbe_offset() + 8));
    uint32_t flags = *((const uint32_t*)(_buffer.data() + _buffer.offset() + fbe_offset() + 12));

    // Calculate decimal value
    double dValue = ((double)low + (double)high * ds2to64) / pow(10.0, (uint8_t)(flags >> 16));
    if (flags & 0x80000000)
        dValue = -dValue;

    value = dValue;
    return fbe_size();
}

size_t FinalModel<decimal_t>::set(decimal_t value) noexcept
{
    assert(((_buffer.offset() + fbe_offset() + fbe_size()) <= _buffer.size()) && "Model is broken!");
    if ((_buffer.offset() + fbe_offset() + fbe_size()) > _buffer.size())
        return 0;

    // The most we can scale by is 10^28, which is just slightly more
    // than 2^93.  So a float with an exponent of -94 could just
    // barely reach 0.5, but smaller exponents will always round to zero.
    const uint32_t DBLBIAS = 1022;

    // Get exponent value
    double dValue = (double)value;
    int32_t iExp = (int32_t)(((uint32_t)(extract(dValue) >> 52) & 0x7FFu) - DBLBIAS);
    if ((iExp < -94) || (iExp > 96))
    {
        // Value too big for .NET Decimal (exponent is limited to [-94, 96])
        memset((uint8_t*)(_buffer.data() + _buffer.offset() + fbe_offset()), 0, 16);
        return fbe_size();
    }

    uint32_t flags = 0;
    if (dValue < 0)
    {
        dValue = -dValue;
        flags = 0x80000000;
    }

    // Round the input to a 15-digit integer.  The R8 format has
    // only 15 digits of precision, and we want to keep garbage digits
    // out of the Decimal were making.

    // Calculate max power of 10 input value could have by multiplying
    // the exponent by log10(2).  Using scaled integer multiplcation,
    // log10(2) * 2 ^ 16 = .30103 * 65536 = 19728.3.
    int32_t iPower = 14 - ((iExp * 19728) >> 16);

    // iPower is between -14 and 43
    if (iPower >= 0)
    {
        // We have less than 15 digits, scale input up.
        if (iPower > 28)
            iPower = 28;

        dValue *= pow(10.0, iPower);
    }
    else
    {
        if ((iPower != -1) || (dValue >= 1E15))
            dValue /= pow(10.0, -iPower);
        else
            iPower = 0; // didn't scale it
    }

    assert(dValue < 1E15);
    if ((dValue < 1E14) && (iPower < 28))
    {
        dValue *= 10;
        iPower++;
        assert(dValue >= 1E14);
    }

    // Round to int64
    uint64_t ulMant;
    ulMant = (uint64_t)(int64_t)dValue;
    dValue -= (int64_t)ulMant; // difference between input & integer
    if ((dValue > 0.5) || ((dValue == 0.5) && ((ulMant & 1) != 0)))
        ulMant++;

    if (ulMant == 0)
    {
        // Mantissa is 0
        memset((uint8_t*)(_buffer.data() + _buffer.offset() + fbe_offset()), 0, 16);
        return fbe_size();
    }

    if (iPower < 0)
    {
        // Add -iPower factors of 10, -iPower <= (29 - 15) = 14
        iPower = -iPower;
        if (iPower < 10)
        {
            double pow10 = (double)powl(10.0, iPower);
            uint64_t low64 = uint32x32((uint32_t)ulMant, (uint32_t)pow10);
            uint64_t high64 = uint32x32((uint32_t)(ulMant >> 32), (uint32_t)pow10);
            *((uint32_t*)(_buffer.data() + _buffer.offset() + fbe_offset())) = (uint32_t)low64;
            high64 += low64 >> 32;
            *((uint32_t*)(_buffer.data() + _buffer.offset() + fbe_offset() + 4)) = (uint32_t)high64;
            high64 >>= 32;
            *((uint32_t*)(_buffer.data() + _buffer.offset() + fbe_offset() + 8)) = (uint32_t)high64;
        }
        else
        {
            // Have a big power of 10.
            assert(iPower <= 14);
            uint64_t low64;
            uint32_t high32;
            uint64x64(ulMant, (uint64_t)pow(10.0, iPower), low64, high32);
            *((uint64_t*)(_buffer.data() + _buffer.offset() + fbe_offset())) = low64;
            *((uint32_t*)(_buffer.data() + _buffer.offset() + fbe_offset() + 8)) = high32;
        }
    }
    else
    {
        // Factor out powers of 10 to reduce the scale, if possible.
        // The maximum number we could factor out would be 14.  This
        // comes from the fact we have a 15-digit number, and the
        // MSD must be non-zero -- but the lower 14 digits could be
        // zero.  Note also the scale factor is never negative, so
        // we can't scale by any more than the power we used to
        // get the integer.
        int lmax = iPower;
        if (lmax > 14)
            lmax = 14;

        if ((((uint8_t)ulMant) == 0) && (lmax >= 8))
        {
            const uint32_t den = 100000000;
            uint64_t div = ulMant / den;
            if ((uint32_t)ulMant == (uint32_t)(div * den))
            {
                ulMant = div;
                iPower -= 8;
                lmax -= 8;
            }
        }

        if ((((uint32_t)ulMant & 0xF) == 0) && (lmax >= 4))
        {
            const uint32_t den = 10000;
            uint64_t div = ulMant / den;
            if ((uint32_t)ulMant == (uint32_t)(div * den))
            {
                ulMant = div;
                iPower -= 4;
                lmax -= 4;
            }
        }

        if ((((uint32_t)ulMant & 3) == 0) && (lmax >= 2))
        {
            const uint32_t den = 100;
            uint64_t div = ulMant / den;
            if ((uint32_t)ulMant == (uint32_t)(div * den))
            {
                ulMant = div;
                iPower -= 2;
                lmax -= 2;
            }
        }

        if ((((uint32_t)ulMant & 1) == 0) && (lmax >= 1))
        {
            const uint32_t den = 10;
            uint64_t div = ulMant / den;
            if ((uint32_t)ulMant == (uint32_t)(div * den))
            {
                ulMant = div;
                iPower--;
            }
        }

        flags |= (uint32_t)iPower << 16;

        *((uint64_t*)(_buffer.data() + _buffer.offset() + fbe_offset())) = ulMant;
        *((uint32_t*)(_buffer.data() + _buffer.offset() + fbe_offset() + 8)) = 0;
    }

    *((uint32_t*)(_buffer.data() + _buffer.offset() + fbe_offset() + 12)) = flags;
    return fbe_size();
}

size_t FinalModel<uuid_t>::verify() const noexcept
{
    if ((_buffer.offset() + fbe_offset() + fbe_size()) > _buffer.size())
        return std::numeric_limits<std::size_t>::max();

    return fbe_size();
}

size_t FinalModel<uuid_t>::get(uuid_t& value) const noexcept
{
    if ((_buffer.offset() + fbe_offset() + fbe_size()) > _buffer.size())
        return 0;

    std::memcpy(value.data().data(), (const uint8_t*)(_buffer.data() + _buffer.offset() + fbe_offset()), fbe_size());
    return fbe_size();
}

size_t FinalModel<uuid_t>::set(uuid_t value) noexcept
{
    assert(((_buffer.offset() + fbe_offset() + fbe_size()) <= _buffer.size()) && "Model is broken!");
    if ((_buffer.offset() + fbe_offset() + fbe_size()) > _buffer.size())
        return 0;

    std::memcpy((uint8_t*)(_buffer.data() + _buffer.offset() + fbe_offset()), value.data().data(), fbe_size());
    return fbe_size();
}

size_t FinalModel<buffer_t>::verify() const noexcept
{
    if ((_buffer.offset() + fbe_offset() + 4) > _buffer.size())
        return std::numeric_limits<std::size_t>::max();

    uint32_t fbe_bytes_size = *((const uint32_t*)(_buffer.data() + _buffer.offset() + fbe_offset()));
    if ((_buffer.offset() + fbe_offset() + 4 + fbe_bytes_size) > _buffer.size())
        return std::numeric_limits<std::size_t>::max();

    return 4 + fbe_bytes_size;
}

size_t FinalModel<buffer_t>::get(void* data, size_t size) const noexcept
{
    assert(((size == 0) || (data != nullptr)) && "Invalid buffer!");
    if ((size > 0) && (data == nullptr))
        return 0;

    assert(((_buffer.offset() + fbe_offset() + 4) <= _buffer.size()) && "Model is broken!");
    if ((_buffer.offset() + fbe_offset() + 4) > _buffer.size())
        return 0;

    uint32_t fbe_bytes_size = *((const uint32_t*)(_buffer.data() + _buffer.offset() + fbe_offset()));
    assert(((_buffer.offset() + fbe_offset() + 4 + fbe_bytes_size) <= _buffer.size()) && "Model is broken!");
    if ((_buffer.offset() + fbe_offset() + 4 + fbe_bytes_size) > _buffer.size())
        return 4;

    size_t result = std::min(size, (size_t)fbe_bytes_size);
    memcpy(data, (const char*)(_buffer.data() + _buffer.offset() + fbe_offset() + 4), result);
    return 4 + fbe_bytes_size;
}

size_t FinalModel<buffer_t>::get(std::vector<uint8_t>& value) const noexcept
{
    value.clear();

    assert(((_buffer.offset() + fbe_offset() + 4) <= _buffer.size()) && "Model is broken!");
    if ((_buffer.offset() + fbe_offset() + 4) > _buffer.size())
        return 0;

    uint32_t fbe_bytes_size = *((const uint32_t*)(_buffer.data() + _buffer.offset() + fbe_offset()));
    assert(((_buffer.offset() + fbe_offset() + 4 + fbe_bytes_size) <= _buffer.size()) && "Model is broken!");
    if ((_buffer.offset() + fbe_offset() + 4 + fbe_bytes_size) > _buffer.size())
        return 4;

    const char* fbe_bytes = (const char*)(_buffer.data() + _buffer.offset() + fbe_offset() + 4);
    value.assign(fbe_bytes, fbe_bytes + fbe_bytes_size);
    return 4 + fbe_bytes_size;
}

size_t FinalModel<buffer_t>::set(const void* data, size_t size)
{
    assert(((size == 0) || (data != nullptr)) && "Invalid buffer!");
    if ((size > 0) && (data == nullptr))
        return 0;

    assert(((_buffer.offset() + fbe_offset() + 4) <= _buffer.size()) && "Model is broken!");
    if ((_buffer.offset() + fbe_offset() + 4) > _buffer.size())
        return 0;

    uint32_t fbe_bytes_size = (uint32_t)size;
    assert(((_buffer.offset() + fbe_offset() + 4 + fbe_bytes_size) <= _buffer.size()) && "Model is broken!");
    if ((_buffer.offset() + fbe_offset() + 4 + fbe_bytes_size) > _buffer.size())
        return 4;

    *((uint32_t*)(_buffer.data() + _buffer.offset() + fbe_offset())) = fbe_bytes_size;

    memcpy((char*)(_buffer.data() + _buffer.offset() + fbe_offset() + 4), data, fbe_bytes_size);
    return 4 + fbe_bytes_size;
}

size_t FinalModel<std::string>::verify() const noexcept
{
    if ((_buffer.offset() + fbe_offset() + 4) > _buffer.size())
        return std::numeric_limits<std::size_t>::max();

    uint32_t fbe_string_size = *((const uint32_t*)(_buffer.data() + _buffer.offset() + fbe_offset()));
    if ((_buffer.offset() + fbe_offset() + 4 + fbe_string_size) > _buffer.size())
        return std::numeric_limits<std::size_t>::max();

    return 4 + fbe_string_size;
}

size_t FinalModel<std::string>::get(char* data, size_t size) const noexcept
{
    assert(((size == 0) || (data != nullptr)) && "Invalid buffer!");
    if ((size > 0) && (data == nullptr))
        return 0;

    assert(((_buffer.offset() + fbe_offset() + 4) <= _buffer.size()) && "Model is broken!");
    if ((_buffer.offset() + fbe_offset() + 4) > _buffer.size())
        return 0;

    uint32_t fbe_string_size = *((const uint32_t*)(_buffer.data() + _buffer.offset() + fbe_offset()));
    assert(((_buffer.offset() + fbe_offset() + 4 + fbe_string_size) <= _buffer.size()) && "Model is broken!");
    if ((_buffer.offset() + fbe_offset() + 4 + fbe_string_size) > _buffer.size())
        return 4;

    size_t result = std::min(size, (size_t)fbe_string_size);
    memcpy(data, (const char*)(_buffer.data() + _buffer.offset() + fbe_offset() + 4), result);
    return 4 + fbe_string_size;
}

size_t FinalModel<std::string>::get(std::string& value) const noexcept
{
    value.clear();

    assert(((_buffer.offset() + fbe_offset() + 4) <= _buffer.size()) && "Model is broken!");
    if ((_buffer.offset() + fbe_offset() + 4) > _buffer.size())
        return 0;

    uint32_t fbe_string_size = *((const uint32_t*)(_buffer.data() + _buffer.offset() + fbe_offset()));
    assert(((_buffer.offset() + fbe_offset() + 4 + fbe_string_size) <= _buffer.size()) && "Model is broken!");
    if ((_buffer.offset() + fbe_offset() + 4 + fbe_string_size) > _buffer.size())
        return 4;

    value.assign((const char*)(_buffer.data() + _buffer.offset() + fbe_offset() + 4), fbe_string_size);
    return 4 + fbe_string_size;
}

size_t FinalModel<std::string>::set(const char* data, size_t size)
{
    assert(((size == 0) || (data != nullptr)) && "Invalid buffer!");
    if ((size > 0) && (data == nullptr))
        return 0;

    assert(((_buffer.offset() + fbe_offset() + 4) <= _buffer.size()) && "Model is broken!");
    if ((_buffer.offset() + fbe_offset() + 4) > _buffer.size())
        return 0;

    uint32_t fbe_string_size = (uint32_t)size;
    assert(((_buffer.offset() + fbe_offset() + 4 + fbe_string_size) <= _buffer.size()) && "Model is broken!");
    if ((_buffer.offset() + fbe_offset() + 4 + fbe_string_size) > _buffer.size())
        return 4;

    *((uint32_t*)(_buffer.data() + _buffer.offset() + fbe_offset())) = fbe_string_size;

    memcpy((char*)(_buffer.data() + _buffer.offset() + fbe_offset() + 4), data, fbe_string_size);
    return 4 + fbe_string_size;
}

size_t FinalModel<std::string>::set(const std::string& value)
{
    assert(((_buffer.offset() + fbe_offset() + 4) <= _buffer.size()) && "Model is broken!");
    if ((_buffer.offset() + fbe_offset() + 4) > _buffer.size())
        return 0;

    uint32_t fbe_string_size = (uint32_t)value.size();
    assert(((_buffer.offset() + fbe_offset() + 4 + fbe_string_size) <= _buffer.size()) && "Model is broken!");
    if ((_buffer.offset() + fbe_offset() + 4 + fbe_string_size) > _buffer.size())
        return 4;

    *((uint32_t*)(_buffer.data() + _buffer.offset() + fbe_offset())) = fbe_string_size;

    memcpy((char*)(_buffer.data() + _buffer.offset() + fbe_offset() + 4), value.data(), fbe_string_size);
    return 4 + fbe_string_size;
}

} // namespace FBE
