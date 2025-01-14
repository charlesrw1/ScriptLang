#pragma once
#include <cstdint>
#include <cstddef>
#include <cstring>
#ifdef _DEBUG
#include <cassert>
#endif

// from vkguide
namespace StringUtils {

	// FNV-1a 32bit hashing algorithm.
	constexpr uint32_t fnv1a_32(char const* s, std::size_t count)
	{
		return ((count ? fnv1a_32(s, count - 1) : 2166136261u) ^ s[count]) * 16777619u;
	}

	constexpr uint64_t fnv1a_64(const char const* s, std::size_t count)
	{
		return ((count ? fnv1a_64(s, count - 1) : 14695981039346656037ull /* offset */) ^ s[count]) * 1099511628211ull /* prime */;
	}

	constexpr size_t const_strlen(const char* s)
	{
		size_t size = 0;
		while (s[size]) { size++; };
		return size;
	}
}

template<int BUFSIZE>
class Stack_String
{
public:
	Stack_String() {
		data_len = 0;
		data[0] = 0;
	}
	Stack_String(const char* str) {
		int s = strlen(str);
		if (s + 1 < BUFSIZE) {
			memcpy(data, str, s);
			data[s] = 0;
			data_len = s;
		}
	}
	Stack_String(const char* str, int len) {
		int s = strlen(str);
		if (len < s) s = len;
		if (s + 1 < BUFSIZE) {
			memcpy(data, str, s);
			data[s] = 0;
			data_len = s;
		}
	}
	int size() { return data_len; }
	const char* c_str() { return data; }

	char* get_data() { return data; }
private:
	int data_len = 0;
	char data[BUFSIZE];
};

class StringView
{
public:
	StringView() {}
	StringView(const char* str) {
		str_start = str;
		str_len = strlen(str);
	}
	StringView(const char* str, int len) {
		str_start = str;
		str_len = len;

#ifdef _DEBUG
		for (int i = 0; i < len-1; i++) {
			assert(str_start[i] != '\0');
		}
#endif // _DEBUG

	}

	bool cmp(const char* other) const {
		const char* p1 = str_start;
		int count = 0;
		while (*other) {
			if (count > str_len) return false;
			if (*p1 != *other) return false;
			p1++;
			other++;
			count++;
		}
		return count == str_len;
	}

	Stack_String<256> to_stack_string() {
		return Stack_String<256>(str_start, str_len);
	}

	const char* str_start = nullptr;
	int str_len = 0;
};