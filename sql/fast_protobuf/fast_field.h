#pragma once
#include <map>
#include <string>
#include <unordered_map>
#include <set>
#include <stdint.h>
#include "varint.h"
#include "fast_common.h"

class proto_field {
public:
#pragma region serialize
	inline static size_t serialize_(bool *data, unsigned char *dest, unsigned char *end) {
		OVERFLOW_CHECK(dest, end, SIZE_1_BYTE);
		memcpy(dest, data, SIZE_1_BYTE);
		return SIZE_1_BYTE;
	}

	inline static size_t serialize_(uint8_t *data, unsigned char *dest, unsigned char *end) {
		OVERFLOW_CHECK(dest, end, SIZE_1_BYTE);
		memcpy(dest, data, SIZE_1_BYTE);
		return SIZE_1_BYTE;
	}

	inline static size_t serialize_(uint16_t *data, unsigned char *dest, unsigned char *end) {
		OVERFLOW_CHECK(dest, end, varint_size(*(uint32_t*)data));
		return varint_pack(*(uint32_t*)data, dest);
	}

	inline static size_t serialize_(uint32_t *data, unsigned char *dest, unsigned char *end) {
		OVERFLOW_CHECK(dest, end, varint_size(*data));
		return varint_pack(*data, dest);
	}

	inline static size_t serialize_(int8_t *data, unsigned char *dest, unsigned char *end) {
		OVERFLOW_CHECK(dest, end, SIZE_1_BYTE);
		memcpy(dest, data, SIZE_1_BYTE);
		return SIZE_1_BYTE;
	}

	inline static size_t serialize_(int16_t *data, unsigned char *dest, unsigned char *end) {
		OVERFLOW_CHECK(dest, end, varint_size(zigZag_encode(*(int32_t*)data)));
		return varint_pack(zigZag_encode(*(int32_t*)data), dest);
	}

	inline static size_t serialize_(int32_t *data, unsigned char *dest, unsigned char *end) {
		uint32_t zig = zigZag_encode(*data);
		OVERFLOW_CHECK(dest, end, varint_size(zig));
		return varint_pack(zig, dest);
	}

	inline static size_t serialize_(uint64_t *data, unsigned char *dest, unsigned char *end) {
		OVERFLOW_CHECK(dest, end, varint_size(*data));
		return varint_pack(*data, dest);
	}

	inline static size_t serialize_(int64_t *data, unsigned char *dest, unsigned char *end) {
		uint64_t zig = zigZag_encode(*data);
		OVERFLOW_CHECK(dest, end, varint_size(zig));
		return varint_pack(zig, dest);
	}

	inline static size_t serialize_(string *data, unsigned char *dest, unsigned char *end) {
		unsigned char *next = dest;
		size_t size = data->size();
		OVERFLOW_CHECK(dest, end, varint_size(size));
		next += varint_pack(size, next);
		if ((next + size) > end) return 0;
		memcpy(next, data->c_str(), size);
		return next - dest + size;
	}

	inline static size_t serialize_(double *data, unsigned char *dest, unsigned char *end) {
		OVERFLOW_CHECK(dest, end, SIZE_8_BYTE);
		memcpy(dest, data, SIZE_8_BYTE);
		return SIZE_8_BYTE;
	}

	inline static size_t serialize_(float *data, unsigned char *dest, unsigned char *end) {
		OVERFLOW_CHECK(dest, end, SIZE_4_BYTE);
		memcpy(dest, data, SIZE_4_BYTE);
		return SIZE_4_BYTE;
	}

	inline static size_t serialize_(fixed64 *data, unsigned char *dest, unsigned char *end) {
		OVERFLOW_CHECK(dest, end, SIZE_8_BYTE);
		memcpy(dest, data, SIZE_8_BYTE);
		return SIZE_8_BYTE;
	}

	inline static size_t serialize_(fixed32 *data, unsigned char *dest, unsigned char *end) {
		OVERFLOW_CHECK(dest, end, SIZE_4_BYTE);
		memcpy(dest, data, SIZE_4_BYTE);
		return SIZE_4_BYTE;
	}

	template<class T>
	inline static size_t serialize_(enumm<T> *data, unsigned char *dest, unsigned char *end) {
		return serialize_(&(data->value), dest, end);
	}

#ifdef USE_TAG
	template<class T1>
	inline static size_t serialize_(proto_base<T1> *data, unsigned char *dest, unsigned char *end) {
		size_t size = data->size();
		OVERFLOW_CHECK(dest, end, varint_size(size));
		unsigned char *next = dest;
		next += varint_pack(size, next);
		next += data->serialize(next, end - next);
		return next - dest;
	}

	template<class T1>
	inline static size_t serialize_(vector<T1> *data, unsigned char *dest, unsigned char *end) {
		size_t size = 0;
		for (auto &ele : *data) {
			size += proto_field::size_(&ele);
		}
		OVERFLOW_CHECK(dest, end, varint_size(size));
		unsigned char *next = dest;
		next += varint_pack(size, next);
		for (auto &ele : *data) {
			next += proto_field::serialize_(&ele, next, end);
		}
		return next - dest;
	}

	template<class T1, class T2>
	inline static size_t serialize_(map<T1, T2> *data, unsigned char *dest, unsigned char *end) {
		size_t size = 0;
		for (auto &ele : *data) {
			size += proto_field::size_((T1*)&ele.first);
			size += proto_field::size_(&ele.second);
		}
		OVERFLOW_CHECK(dest, end, varint_size(size));
		unsigned char *next = dest;
		next += varint_pack(size, next);
		for (auto &ele : *data) {
			next += proto_field::serialize_((T1*)&ele.first, next, end);
			next += proto_field::serialize_(&ele.second, next, end);
		}
		return next - dest;
	}

	template<class T1, class T2>
	inline static size_t serialize_(unordered_map<T1, T2> *data, unsigned char *dest, unsigned char *end) {
		size_t size = 0;
		for (auto &ele : *data) {
			size += proto_field::size_((T1*)&ele.first);
			size += proto_field::size_(&ele.second);
		}
		OVERFLOW_CHECK(dest, end, varint_size(size));
		unsigned char *next = dest;
		next += varint_pack(size, next);
		for (auto &ele : *data) {
			next += proto_field::serialize_((T1*)&ele.first, next, end);
			next += proto_field::serialize_(&ele.second, next, end);
		}
		return next - dest;
	}

	template<class T1>
	inline static size_t serialize_(set<T1> *data, unsigned char *dest, unsigned char *end) {
		size_t size = 0;
		for (auto &ele : *data) {
			size += proto_field::size_((T1*)&ele);
		}
		OVERFLOW_CHECK(dest, end, varint_size(size));
		unsigned char *next = dest;
		next += varint_pack(size, next);
		for (auto &ele : *data) {
			next += proto_field::serialize_((T1*)&ele, next, end);
		}
		return next - dest;
	}

#else
	template<class T1>
	inline static size_t serialize_(proto_base<T1> *data, unsigned char *dest, unsigned char *end) {
		return data->serialize(dest, end - dest);
	}

	template<class T1>
	inline static size_t serialize_(vector<T1> *data, unsigned char *dest, unsigned char *end) {
		size_t size = data->size();
		OVERFLOW_CHECK(dest, end, varint_size(size));
		unsigned char *next = dest;
		next += varint_pack(size, next);
		for (auto &ele : *data) {
			next += proto_field::serialize_(&ele, next, end);
		}
		return next - dest;
	}

	template<class T1, class T2>
	inline static size_t serialize_(map<T1, T2> *data, unsigned char *dest, unsigned char *end) {
		size_t size = data->size();
		OVERFLOW_CHECK(dest, end, varint_size(size));
		unsigned char *next = dest;
		next += varint_pack(size, next);
		for (auto &ele : *data) {
			next += proto_field::serialize_((T1*)&ele.first, next, end);
			next += proto_field::serialize_(&ele.second, next, end);
		}
		return next - dest;
	}

	template<class T1, class T2>
	inline static size_t serialize_(unordered_map<T1, T2> *data, unsigned char *dest, unsigned char *end) {
		size_t size = data->size();
		OVERFLOW_CHECK(dest, end, varint_size(size));
		unsigned char *next = dest;
		next += varint_pack(size, next);
		for (auto &ele : *data) {
			next += proto_field::serialize_((T1*)&ele.first, next, end);
			next += proto_field::serialize_(&ele.second, next, end);
		}
		return next - dest;
	}

	template<class T1>
	inline static size_t serialize_(set<T1> *data, unsigned char *dest, unsigned char *end) {
		size_t size = data->size();
		OVERFLOW_CHECK(dest, end, varint_size(size));
		unsigned char *next = dest;
		next += varint_pack(size, next);
		for (auto &ele : *data) {
			next += proto_field::serialize_((T1*)&ele, next, end);
		}
		return next - dest;
	}

#endif // USE_TAG


#pragma endregion

#pragma region unserialize
	inline static size_t unserialize_(bool *data, const unsigned char *dest, const unsigned char *end) {
		OVERFLOW_CHECK(dest, end, SIZE_1_BYTE);
		memcpy(data, dest, SIZE_1_BYTE);
		return SIZE_1_BYTE;
	}

	inline static size_t unserialize_(uint8_t *data, const unsigned char *dest, const unsigned char *end) {
		OVERFLOW_CHECK(dest, end, SIZE_1_BYTE);
		memcpy(data, dest, SIZE_1_BYTE);
		return SIZE_1_BYTE;
	}

	inline static size_t unserialize_(uint16_t *data, const unsigned char *dest, const unsigned char *end) {
		OVERFLOW_CHECK(dest, end, varint_scan(dest));
		return varint_unpack(dest, (uint32_t*)data);
	}

	inline static size_t unserialize_(uint32_t *data, const unsigned char *dest, const unsigned char *end) {
		OVERFLOW_CHECK(dest, end, varint_scan(dest));
		return varint_unpack(dest, data);
	}

	inline static size_t unserialize_(int8_t *data, const unsigned char *dest, const unsigned char *end) {
		OVERFLOW_CHECK(dest, end, varint_scan(dest));
		memcpy(data, dest, SIZE_1_BYTE);
		return SIZE_1_BYTE;
	}

	inline static size_t unserialize_(int16_t *data, const unsigned char *dest, const unsigned char *end) {
		OVERFLOW_CHECK(dest, end, varint_scan(dest));
		const unsigned char *next = dest;
		uint32_t re;
		size_t size = varint_unpack(next, &re);
		*data = zigZag_decode(re);
		return size;
	}

	inline static size_t unserialize_(int32_t *data, const unsigned char *dest, const unsigned char *end) {
		OVERFLOW_CHECK(dest, end, varint_scan(dest));
		uint32_t re;
		size_t size = varint_unpack(dest, &re);
		*data = zigZag_decode(re);
		return size;
	}

	inline static size_t unserialize_(uint64_t *data, const unsigned char *dest, const unsigned char *end) {
		OVERFLOW_CHECK(dest, end, varint_scan(dest));
		return varint_unpack(dest, data);
	}

	inline static size_t unserialize_(int64_t *data, const unsigned char *dest, const unsigned char *end) {
		OVERFLOW_CHECK(dest, end, varint_scan(dest));
		uint64_t re;
		size_t size = varint_unpack(dest, &re);
		*data = zigZag_decode(re);
		return size;
	}

	inline static size_t unserialize_(string *data, const unsigned char *dest, const unsigned char *end) {
		OVERFLOW_CHECK(dest, end, varint_scan(dest));
		const unsigned char *next = dest;
		uint32_t size;
		next += varint_unpack(next, &size);

		OVERFLOW_CHECK(next, end, size);
		data->assign((char*)next, size);
		return next - dest + size;
	}

	inline static size_t unserialize_(double *data, const unsigned char *dest, const unsigned char *end) {
		OVERFLOW_CHECK(dest, end, SIZE_8_BYTE);
		memcpy(data, dest, SIZE_8_BYTE);

		return SIZE_8_BYTE;
	}

	inline static size_t unserialize_(float *data, const unsigned char *dest, const unsigned char *end) {
		OVERFLOW_CHECK(dest, end, SIZE_4_BYTE);
		memcpy(data, dest, SIZE_4_BYTE);
		return SIZE_4_BYTE;
	}

	inline static size_t unserialize_(fixed64 *data, const unsigned char *dest, const unsigned char *end) {
		OVERFLOW_CHECK(dest, end, SIZE_8_BYTE);
		memcpy(data, dest, SIZE_8_BYTE);
		return SIZE_8_BYTE;
	}

	inline static size_t unserialize_(fixed32 *data, const unsigned char *dest, const unsigned char *end) {
		OVERFLOW_CHECK(dest, end, SIZE_4_BYTE);
		memcpy(data, dest, SIZE_4_BYTE);
		return SIZE_4_BYTE;
	}

	template<class T>
	inline static size_t unserialize_(enumm<T> *data, const unsigned char *dest, const unsigned char *end) {
		return unserialize_(&(data->value), dest, end);
	}

#ifdef USE_TAG
	template<class T1>
	inline static size_t unserialize_(proto_base<T1> *data, const unsigned char *dest, const unsigned char *end) {
		OVERFLOW_CHECK(dest, end, varint_scan(dest));
		const unsigned char *next = dest;
		size_t len;
		next += varint_unpack(next, &len);
		next += data->unserialize(next, end - next);
		return next - dest;
	}

	template<class T1>
	inline static size_t unserialize_(vector<T1> *data, const unsigned char *dest, const unsigned char *end) {
		OVERFLOW_CHECK(dest, end, varint_scan(dest));
		const unsigned char *next = dest;
		size_t len;
		next += varint_unpack(next, &len);
		const unsigned char *last = next + len;
		data->resize(len);
		//data->reserve(len);
		size_t index = 0;
		while (next < last) {
			next += proto_field::unserialize_(&(*data)[index++], next, end);
		}
		data->resize(index);
		return next - dest;
	}

	template<class T1, class T2>
	inline static size_t unserialize_(map<T1, T2> *data, const unsigned char *dest, const unsigned char *end) {
		OVERFLOW_CHECK(dest, end, varint_scan(dest));
		const unsigned char *next = dest;
		size_t len;
		next += varint_unpack(next, &len);
		const unsigned char *last = next + len;
		T1 key;
		while (next < last) {
			next += proto_field::unserialize_(&key, next, end);

			T2 &val = (*data)[key];
			next += proto_field::unserialize_(&val, next, end);
		}
		return next - dest;
	}

	template<class T1, class T2>
	inline static size_t unserialize_(unordered_map<T1, T2> *data, const unsigned char *dest, const unsigned char *end) {
		OVERFLOW_CHECK(dest, end, varint_scan(dest));
		const unsigned char *next = dest;
		size_t len;
		next += varint_unpack(next, &len);
		const unsigned char *last = next + len;
		T1 key;
		while (next < last) {
			next += proto_field::unserialize_(&key, next, end);

			T2 &val = (*data)[key];
			next += proto_field::unserialize_(&val, next, end);
		}
		return next - dest;
	}

	template<class T1>
	inline static size_t unserialize_(set<T1> *data, const unsigned char *dest, const unsigned char *end) {
		OVERFLOW_CHECK(dest, end, varint_scan(dest));
		const unsigned char *next = dest;
		size_t len;
		next += varint_unpack(next, &len);
		const unsigned char *last = next + len;
		T1 key;
		while (next < last) {
			next += proto_field::unserialize_(&key, next, end);
			data->insert(key);
		}
		return next - dest;
	}

#else
	template<class T1>
	inline static size_t unserialize_(proto_base<T1> *data, const unsigned char *dest, const unsigned char *end) {
		return data->unserialize(dest, end - dest);
	}

	template<class T1>
	inline static size_t unserialize_(vector<T1> *data, const unsigned char *dest, const unsigned char *end) {
		OVERFLOW_CHECK(dest, end, varint_scan(dest));
		const unsigned char *next = dest;
		size_t len;
		next += varint_unpack(next, &len);
		data->resize(len);
		for (auto &ele : *data) {
			next += proto_field::unserialize_(&ele, next, end);
		}
		return next - dest;
	}

	template<class T1, class T2>
	inline static size_t unserialize_(map<T1, T2> *data, const unsigned char *dest, const unsigned char *end) {
		OVERFLOW_CHECK(dest, end, varint_scan(dest));
		const unsigned char *next = dest;
		size_t len;
		next += varint_unpack(next, &len);
		T1 key;
		for (int i = 0; i < len; i++) {
			next += proto_field::unserialize_(&key, next, end);

			T2 &val = (*data)[key];
			next += proto_field::unserialize_(&val, next, end);
		}
		return next - dest;
	}

	template<class T1, class T2>
	inline static size_t unserialize_(unordered_map<T1, T2> *data, const unsigned char *dest, const unsigned char *end) {
		OVERFLOW_CHECK(dest, end, varint_scan(dest));
		const unsigned char *next = dest;
		size_t len;
		next += varint_unpack(next, &len);
		T1 key;
		for (int i = 0; i < len; i++) {
			next += proto_field::unserialize_(&key, next, end);

			T2 &val = (*data)[key];
			next += proto_field::unserialize_(&val, next, end);
		}
		return next - dest;
	}

	template<class T1>
	inline static size_t unserialize_(set<T1> *data, const unsigned char *dest, const unsigned char *end) {
		OVERFLOW_CHECK(dest, end, varint_scan(dest));
		const unsigned char *next = dest;
		size_t len;
		next += varint_unpack(next, &len);
		T1 key;
		for (int i = 0; i < len; i++) {
			next += proto_field::unserialize_(&key, next, end);
			data->insert(key);
		}
		return next - dest;
	}

#endif // USE_TAG

#pragma endregion

#pragma region size
	inline static size_t size_(bool *data) {
		return SIZE_1_BYTE;
	}

	inline static size_t size_(uint8_t *data) {
		return SIZE_1_BYTE;
	}

	inline static size_t size_(uint16_t *data) {
		return varint_size(*(uint32_t*)data);
	}
	inline static size_t size_(uint32_t *data) {
		return varint_size(*data);
	}

	inline static size_t size_(int8_t *data) {
		return SIZE_1_BYTE;
	}

	inline static size_t size_(int16_t *data) {
		return varint_size(zigZag_encode(*(int32_t*)data));
	}

	inline static size_t size_(int32_t *data) {
		return varint_size(zigZag_encode(*data));
	}

	inline static size_t size_(uint64_t *data) {
		return varint_size(*data);
	}

	inline static size_t size_(int64_t *data) {
		return varint_size(zigZag_encode(*(int64_t*)data));
	}

	inline static size_t size_(string *data) {
		size_t size = 0;
		size += varint_size(data->size());
		size += data->size();
		return size;
	}

	inline static size_t size_(double *data) {
		return SIZE_8_BYTE;
	}

	inline static size_t size_(float *data) {
		return SIZE_4_BYTE;
	}

	inline static size_t size_(fixed64 *data) {
		return SIZE_8_BYTE;
	}

	inline static size_t size_(fixed32 *data) {
		return SIZE_4_BYTE;
	}

	template<class T>
	inline static size_t size_(enumm<T> *data) {
		return size_(&(data->value));
	}

	template<class T1>
	inline static size_t size_(proto_base<T1> *data) {
		return data->size();
	}

#ifdef USE_TAG
	template<class T1>
	inline static size_t size_(vector<T1> *data) {
		size_t size = 0;
		for (auto &ele : *data) {
			size += proto_field::size_(&ele);
		}
		size += varint_size(size);
		return size;
	}

	template<class T1, class T2>
	inline static size_t size_(map<T1, T2> *data) {
		size_t size = 0;
		for (auto &ele : *data) {
			size += proto_field::size_((T1*)&ele.first);
			size += proto_field::size_(&ele.second);
		}
		size += varint_size(size);
		return size;
	}

	template<class T1, class T2>
	inline static size_t size_(unordered_map<T1, T2> *data) {
		size_t size = 0;
		for (auto &ele : *data) {
			size += proto_field::size_((T1*)&ele.first);
			size += proto_field::size_(&ele.second);
		}
		size += varint_size(size);
		return size;
	}

	template<class T1>
	inline static size_t size_(set<T1> *data) {
		size_t size = 0;
		for (auto &ele : *data) {
			size += proto_field::size_((T1*)&ele);
		}
		size += varint_size(size);
		return size;
	}

#else
	template<class T1>
	inline static size_t size_(vector<T1> *data) {
		return data->size();
	}

	template<class T1, class T2>
	inline static size_t size_(map<T1, T2> *data) {
		return data->size();
	}

	template<class T1, class T2>
	inline static size_t size_(unordered_map<T1, T2> *data) {
		return data->size();
	}

	template<class T1>
	inline static size_t size_(set<T1> *data) {
		return data->size();
	}
#endif // USE_TAG

#pragma endregion

#pragma region is_default
	inline static bool is_default_(bool *data) {
		CHECK_DEFAULT(*data, false);
	}

	inline static bool is_default_(uint8_t *data) {
		CHECK_DEFAULT(*data, 0);
	}

	inline static bool is_default_(uint16_t *data) {
		CHECK_DEFAULT(*data, 0);
	}
	inline static bool is_default_(uint32_t *data) {
		CHECK_DEFAULT(*data, 0);
	}

	inline static bool is_default_(int8_t *data) {
		CHECK_DEFAULT(*data, 0);
	}

	inline static bool is_default_(int16_t *data) {
		CHECK_DEFAULT(*data, 0);
	}

	inline static bool is_default_(int32_t *data) {
		CHECK_DEFAULT(*data, 0);
	}

	inline static bool is_default_(uint64_t *data) {
		CHECK_DEFAULT(*data, 0);
	}

	inline static bool is_default_(int64_t *data) {
		CHECK_DEFAULT(*data, 0);
	}

	inline static bool is_default_(string *data) {
		CHECK_DEFAULT(data->size(), 0);
	}

	inline static bool is_default_(double *data) {
		CHECK_DEFAULT(*data, 0);
	}

	inline static bool is_default_(float *data) {
		CHECK_DEFAULT(*data, 0);
	}

	inline static bool is_default_(fixed32 *data) {
		CHECK_DEFAULT(*data, 0);
		return false;
	}

	inline static bool is_default_(fixed64 *data) {
		CHECK_DEFAULT(*data, 0);
		return false;
	}

	template<class T>
	inline static bool is_default_(enumm<T> *data) {
		CHECK_DEFAULT(data->value, 0);
		return false;
	}

	template<class T1>
	inline static bool is_default_(proto_base<T1> *data) {
		return data->is_default();
	}

	template<class T1>
	inline static bool is_default_(vector<T1> *data) {
		CHECK_DEFAULT(data->size(), 0);
	}

	template<class T1, class T2>
	inline static bool is_default_(map<T1, T2> *data) {
		CHECK_DEFAULT(data->size(), 0);
	}

	template<class T1, class T2>
	inline static bool is_default_(unordered_map<T1, T2> *data) {
		CHECK_DEFAULT(data->size(), 0);
	}

	template<class T1>
	inline static bool is_default_(set<T1> *data) {
		CHECK_DEFAULT(data->size(), 0);
	}

#pragma endregion

#pragma region refresh
	inline static void refresh_(bool *data) {
		*data = false;
	}

	inline static void refresh_(uint8_t *data) {
		*data = 0;
	}

	inline static void refresh_(uint16_t *data) {
		*data = 0;
	}

	inline static void refresh_(uint32_t *data) {
		*data = 0;
	}

	inline static void refresh_(int8_t *data) {
		*data = 0;
	}

	inline static void refresh_(int16_t *data) {
		*data = 0;
	}

	inline static void refresh_(int32_t *data) {
		*data = 0;
	}

	inline static void refresh_(uint64_t *data) {
		*data = 0;
	}

	inline static void refresh_(int64_t *data) {
		*data = 0;
	}
	inline static void refresh_(string *data) {
		data->clear();
	}

	inline static void refresh_(double *data) {
		*data = 0;
	}

	inline static void refresh_(float *data) {
		*data = 0;
	}

	inline static void refresh_(fixed32 *data) {
		*data = 0;
	}

	inline static void refresh_(fixed64 *data) {
		*data = 0;
	}

	template<class T>
	inline static void refresh_(enumm<T> *data) {
		data->value = 0;
	}

	template<class T1>
	inline static void refresh_(proto_base<T1> *data) {
		data->refresh();
	}

	template<class T1>
	inline static void refresh_(vector<T1> *data) {
		data->clear();
	}

	template<class T1, class T2>
	inline static void refresh_(map<T1, T2> *data) {
		data->clear();
	}

	template<class T1, class T2>
	inline static void refresh_(unordered_map<T1, T2> *data) {
		data->clear();
	}

	template<class T1>
	inline static void refresh_(set<T1> *data) {
		data->clear();
	}

#pragma endregion

#pragma region type
	inline static WireType type_(bool *data) {
		return WireType_Varint;
	}

	inline static WireType type_(uint8_t *data) {
		return WireType_Varint;
	}

	inline static WireType type_(uint16_t *data) {
		return WireType_Varint;
	}

	inline static WireType type_(uint32_t *data) {
		return WireType_Varint;
	}

	inline static WireType type_(int8_t *data) {
		return WireType_Varint;
	}

	inline static WireType type_(int16_t *data) {
		return WireType_Varint;
	}

	inline static WireType type_(int32_t *data) {
		return WireType_Varint;
	}

	inline static WireType type_(uint64_t *data) {
		return WireType_Varint;
	}

	inline static WireType type_(int64_t *data) {
		return WireType_Varint;
	}

	inline static WireType type_(string *data) {
		return WireType_Varint;
	}

	inline static WireType type_(double *data) {
		return WireType_Fix64;
	}

	inline static WireType type_(float *data) {
		return WireType_Fix32;
	}

	inline static WireType type_(fixed32 *data) {
		return WireType_Fix32;;
	}

	inline static WireType type_(fixed64 *data) {
		return WireType_Fix64;
	}

	template<class T>
	inline static WireType type_(enumm<T> *data) {
		return WireType_Varint;
	}

	template<class T1>
	inline static WireType type_(proto_base<T1> *data) {
		return WireType_Length_Limited;
	}

	template<class T1>
	inline static WireType type_(vector<T1> *data) {
		return WireType_Length_Limited;
	}

	template<class T1, class T2>
	inline static WireType type_(map<T1, T2> *data) {
		return WireType_Length_Limited;
	}

	template<class T1, class T2>
	inline static WireType type_(unordered_map<T1, T2> *data) {
		return WireType_Length_Limited;
	}

	template<class T1>
	inline static WireType type_(set<T1> *data) {
		return WireType_Length_Limited;
	}
#pragma endregion
public:
	template<class Container>
	proto_field(Container *container, size_t id, void (Container::*operater)(uint8_t)) {
		container->add_field(id, operater);
	}
};
