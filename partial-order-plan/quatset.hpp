#include <compare>
#include <cstdint>

namespace qy
{
	class quatset
	{
	public:
		using value_type = uint64_t;

		enum {
			UNKNOWN = 0b00, FALSE = 0b01, TRUE = 0b10, BOTH = 0b11
		};

		quatset(value_type v = 0): m_bitset(v) {}

		inline uint8_t get(uint8_t i) const {
			i <<= 1;
			return (m_bitset >> i) & 0b11;
		}

		inline void set(uint8_t i, uint8_t value = TRUE) {
			i <<= 1;
			m_bitset &= ~(0b11 << i);
			m_bitset |= (value & 0b11) << i;
		}

		inline void setb(uint8_t i, bool value = true) {
			set(i, (uint8_t)value + 1);
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


