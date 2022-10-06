#pragma once
#include <array>
#include <iostream>
#include <algorithm>
#include <numeric>

#define USE_CANTOR

using puzzle_size_t = unsigned;

/// @brief Base class of puzzle.
struct puzzle_base {
	/// @brief Direction constants.
	inline constexpr static std::array<std::pair<int, int>, 4> dir{ {{1, 0}, {0, 1}, {-1, 0}, {0, -1}} };
};

/// @brief Puzzle class.
/// @tparam row Row of puzzle.
/// @tparam col Column of puzzle.
template <puzzle_size_t row, puzzle_size_t col>
struct puzzle : puzzle_base {

	/// @brief Total size of puzzle.
	constexpr static puzzle_size_t size = row * col;

	/// @brief Digits in the puzzle.
	std::array<int8_t, size> digits;

	/// @brief Position of zero/blank.
	int8_t zero_pos;

	/// @brief Initialize digits with 0..n-1 by default.
	puzzle() noexcept {
		std::iota(digits.begin(), digits.end(), 0);
		zero_pos = 0;
	}

	/// @brief Construct from array of digits.
	/// @param digits Array of digits.
	puzzle(std::array<int8_t, size> digits) noexcept : digits(digits), zero_pos(-1) {
		get_zero_pos();
	}

	/// @brief Get i-th digit.
	/// @param i Index of digit.
	/// @return The i-th digit.
	inline int8_t operator[] (int i) const { return digits[i]; }

	/// @brief Check whether can move by direction.
	/// @param dx Delta row.
	/// @param dy Delta col.
	/// @return Feasibility.
	bool can_move(int dx, int dy) const {
		int tx = zero_pos / col + dx, ty = zero_pos % col + dy;
		return tx >= 0 && tx < row&& ty >= 0 && ty < col;
	}

	/// @brief Check whether can move by direction.
	/// @param d Direction index.
	/// @return Feasibility.
	bool can_move(int d) const {
		return can_move(dir[d].first, dir[d].second);
	}

	/// @brief Move the blank block by direction.
	/// @param dx Delta row.
	/// @param dy Delta col.
	void move_blank(int dx, int dy) {
		int zx = zero_pos / col, zy = zero_pos % col;
		int tx = zx + dx, ty = zy + dy, tz = tx * col + ty;
		std::swap(digits[zero_pos], digits[tz]);
		zero_pos = tz;
	}

	/// @brief Move the blank block by direction.
	/// @param d Direction index.
	void move_blank(int d) {
		move_blank(dir[d].first, dir[d].second);
	}

	/// @brief Get hash code, or permutation rank.
	/// @return Hash code of the puzzle.
	size_t hash_code() const {
		puzzle_size_t n = size;
		size_t ans = 0, f = 1;
#ifdef USE_CANTOR
		for (int i = n - 1; i >= 0; f *= n - i, i--) {
			int t = 0;
			for (int j = i + 1; j < n; j++)
				if (digits[j] < digits[i]) t++;
			ans += t * f;
		}
#else
		for (int i = 0; i < n; i++, f *= 10) {
			ans += digits[i] * f;
		}
#endif
		return ans;
	}

	friend bool operator== (const puzzle& lhs, const puzzle& rhs) {
		return std::ranges::equal(lhs.digits, rhs.digits);
	}

	friend std::ostream& operator<< (std::ostream& os, const puzzle& p) {
		for (int i = 0; i < row; i++) {
			os << "|";
			for (int j = 0; j < col; j++) {
				os << (int)p[i * col + j] << " |"[j == col - 1];
			}
			os << "\n";
		}
		return os;
	}

private:
	/// @brief Compute the position of zero/blank.
	void get_zero_pos() {
		for (int i = 0; i < size; i++)
			if (digits[i] == 0) zero_pos = i;
	}
};

/// @brief Puzzle with size 3x3.
using puzzle3x3 = puzzle<3, 3>;

/// @brief Get hash code of puzzle using Cantor expansion
/// @param p Puzzle
/// @return Hash code as rank of this permutation.
template <puzzle_size_t row, puzzle_size_t col>
size_t hash(const puzzle<row, col>& p) {
	return p.hash_code();
}

