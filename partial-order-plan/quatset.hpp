#include <compare>

namespace qy
{
	class quatset
	{
	public:
		using value_type = uint64_t;

		enum {
			UNKNOWN = 0b00, FALSE = 0b01, TRUE = 0b10
		};

		inline uint8_t get(uint8_t i) const {
			i <<= 1;
			return (m_bitset >> i) & 0b11;
		}

		inline void set(uint8_t i, uint8_t value = TRUE) {
			i <<= 1;
			m_bitset &= ~(0b11 << i);
			m_bitset |= (value & 0b11) << i;
		}

		inline bool includes(const quatset& other) const {
			return (m_bitset & other.m_bitset) == other.m_bitset;
		}

		bool operator== (const quatset& other) const = default;
		auto operator<=> (const quatset& other) const = default;

	private:
		value_type m_bitset{ 0 };
	};

} // namespace qy


