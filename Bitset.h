#pragma once
#include <type_traits>
#include <bit>
#include <iostream>

constexpr int Ceil(float num)
{
	return (static_cast<float>(static_cast<int>(num)) == num)
		? static_cast<int>(num)
		: static_cast<int>(num) + ((num > 0) ? 1 : 0);
}

template<int BitCount>
class Bitset
{
	using WordT =
		std::conditional_t<BitCount <= 8, uint8_t,
		std::conditional_t<BitCount <= 16, uint16_t,
		std::conditional_t<BitCount <= 32, uint32_t,
		uint64_t>>>;
	static constexpr int WordSize = sizeof(WordT);
	static constexpr int WordBitCount = WordSize * 8;
	static constexpr int WordCount = Ceil((float)BitCount / (float)WordBitCount);
public:
	Bitset()
		: m_Words()
	{}

	Bitset& Set(WordT index, bool value = true)
	{	
		#ifdef DEBUG
		if (index >= BitCount) __debugbreak();
		#endif
		WordT& selectedWord = m_Words[index / WordBitCount];

		selectedWord ^= (WordT)((WordT)(-value) ^ selectedWord) & (WordT)((WordT)1 << (index % WordBitCount));

		#ifdef DEBUG
		m_BitRepresentation[index] = value ? '1' : '0';
		#endif

		return *this;
	}

	bool Test(int index)
	{
		#ifdef DEBUG
		if (index >= BitCount) __debugbreak();
		#endif
		return (m_Words[index / WordBitCount] & ((WordT)1 << (index % WordBitCount))) != 0;
	}

	bool All()
	{
		if constexpr (BitCount == 0)
			return true;
		
		constexpr bool hasPadding = BitCount % WordBitCount > 0;
		for (int i = 0; i < WordCount - hasPadding; i++)
		{
			if (m_Words[i] != ~(WordT)(0))
				return false;
		}
		bool isLastNodeFull = m_Words[WordCount - 1] == (WordT)(((WordT)(1) << (BitCount % WordBitCount)) - 1);
		return !hasPadding || isLastNodeFull;
	}

	bool Any() const noexcept {
		for (int i = 0; i <= WordCount; i++) {
			if (m_Words[i] != 0) 
				return true;
		}

		return false;
	}

	bool None()
	{
		for (int i = 0; i < WordCount; i++) {
			if (m_Words[i] != 0)
				return false;
		}

		return true;
	}

	constexpr int Size()
	{
		return BitCount;
	}

	int FindFirstZeroIndex()
	{
		int wordIndex = 0;
		while (m_Words[wordIndex] == ~(WordT)(0) && wordIndex < WordCount - 1)
		{
			wordIndex++;
		}
		return wordIndex * WordBitCount + std::countr_one(m_Words[wordIndex]);
	}

	int FindFirstOneIndex()
	{
		int wordIndex = 0;
		while (m_Words[wordIndex] == 0 && wordIndex < WordCount - 1)
		{
			wordIndex++;
		}
		return wordIndex * WordBitCount + std::countr_zero(m_Words[wordIndex]);
	}

	std::string ToString()
	{
		std::string str(BitCount, '0');
		for (int i = 0; i < BitCount; i++)
		{
			if (Test(i))
				str[i] = '1';
		}
		return str;
	}

private:
	#ifdef DEBUG
	std::string m_BitRepresentation = std::string(BitCount, '0');
	#endif
	WordT m_Words[WordCount];
};