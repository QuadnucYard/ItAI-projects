#include <compare>
#include <cstdint>

namespace qy
{
	class quatset
	{
	public:
		using value_type = uint64_t;
		using index_t = uint8_t;
		using bits_t = uint8_t;

		enum {
			UNKNOWN = 0b00, FALSE = 0b01, TRUE = 0b10, BOTH = 0b11
		};

		quatset(value_type v = 0): m_bitset(v) {}

		inline bits_t get(index_t i) const {
			i <<= 1;
			return (m_bitset >> i) & 0b11;
		}

		inline void set(index_t i, bits_t value = TRUE) {
			i <<= 1;
			m_bitset &= ~((value_type)0b11 << i);
			m_bitset |= (value_type)(value & 0b11) << i;
		}

		inline void setb(index_t i, bool value = true) {
			set(i, (bits_t)value + 1);
		}

		inline void modify(quatset value, quatset mask) {
			m_bitset = (m_bitset & ~mask.m_bitset) | value.m_bitset;
		}

		inline quatset modified(quatset value, quatset mask) const {
			return { (m_bitset & ~mask.m_bitset) | value.m_bitset };
		}

		inline bool includes(quatset other) const {
			return (m_bitset & other.m_bitset) == other.m_bitset;
		}

		inline quatset unk_as_false(index_t nbits) const {
			quatset ret = *this;
			for (index_t i = 0; i < nbits; i++)
				if (!ret.get(i))
					ret.set(i, FALSE);
			return ret;
		}

		inline quatset operator~ () const {
			return { ~m_bitset };
		}

		inline quatset operator& (quatset other) const {
			return { m_bitset & other.m_bitset };
		}

		inline quatset operator| (quatset other) const {
			return { m_bitset | other.m_bitset };
		}

		inline operator bool() const {
			return m_bitset;
		}

		bool operator== (const quatset& other) const = default;
		auto operator<=> (const quatset& other) const = default;

	private:
		value_type m_bitset;
	};

} // namespace qy


