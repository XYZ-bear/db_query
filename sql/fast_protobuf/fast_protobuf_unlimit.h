#pragma once

#if defined(SPEED_UNLIMITE)

#include <vector>
#include "fast_common.h"
#include "fast_field.h"
using namespace std;

#ifdef ele
#undef ele
#endif

#ifdef MESSAGE
#undef MESSAGE
#endif

#ifdef UNION_TYPE3
#undef UNION_TYPE3
#endif

#ifdef UNION_TYPE4
#undef UNION_TYPE4
#endif

#ifdef enum
#undef enum
#endif

#define enum(name,...) enum name{__VA_ARGS__};

#define UNION_TYPE3(t1, name, id) \
							private:\
								const proto_field field_##id{this,id,&message_type::operater_##id};\
							public:\
								t1 name;\
								OVERRIDE_OPERATER(name,id,proto_field::type_(&name));

#define UNION_TYPE4(t1, t2, name, id) \
							private:\
								const proto_field field_##id{this,id,&message_type::operater_##id};\
							public:\
								t1,##t2 name;\
								OVERRIDE_OPERATER(name,id,proto_field::type_(&name));

#define ele(...) INVOKE_VAR_MACRO(UNION_TYPE, __VA_ARGS__);

#define MESSAGE(class_name)\
	class class_name:public proto_base<class_name>\

template<class T>
class proto_base
{
	typedef T message_type;
	typedef void(T::*operater)(uint8_t);
protected:
	size_t size_tag_(id_type id, WireType type) {
		id_type key = (id << WIRE_TYPE_LEN) | (id_type)type;
		return varint_size(key);
	}

	size_t serialize_tag_(id_type id, WireType type) {
		id_type key = (id << WIRE_TYPE_LEN) | (id_type)type;
		size_t size = varint_pack(key, next);
		next += size;
		return size;
	}

	size_t unserialize_tag_(id_type id, WireType type) {
		id_type key;
		size_t tag_size = varint_unpack(unnext, &key);
		id_type iid = key >> WIRE_TYPE_LEN;
		id_type kTagTypeMask = (1 << WIRE_TYPE_LEN) - 1;
		WireType ttype = WireType(key & kTagTypeMask);
		if (iid == id) {
			unnext += tag_size;
			if (type == ttype)
				return tag_size;
			else {
				if (ttype == WireType_Varint) {
					unnext += varint_scan(unnext);
				}
				else if (ttype == WireType_Fix32) {
					unnext += SIZE_4_BYTE;
				}
				else if (ttype == WireType_Fix64) {
					unnext += SIZE_8_BYTE;
				}
				else {
					size_t len = 0;
					unnext += (varint_unpack(unnext, &len) + len);
				}
				return 0;
			}
		}
		else {
			return 0;
		}
	}
private:
	void op(uint8_t o) {
		T *entity = (T*)this;
		for (auto field : fields) {
			if (field)
				(entity->*field)(o);
			else
				break;
		}
	}
public:
	size_t size() {
		_size = 0;
		op(OP_SIZE);
		return _size;
	}
	static T &instance() {
		static T entity;
		return entity;
	}
	static T &refresh_instance() {
		static T entity;
		refresh();
		return entity;
	}
	void refresh() {
		op(OP_REFRESH);
	}
	bool is_default() {
		_is_default = true;
		op(OP_DEFAULT);
		return _is_default;
	}
	void add_field(size_t id, operater field) {
		if (id > fields.size()) {
			fields.resize(id);
			fields[id - 1] = field;
		}
		//fields.push_back(field);
	}
	size_t serialize(unsigned char *nex, size_t size) {
		next = nex;
		end = next + size;
		op(OP_SERIALIZE);
		return next - nex;
	}

	size_t unserialize(const unsigned char *nex, size_t size) {
		unnext = nex;
		unend = unnext + size;
		op(OP_UNSERIALIZE);
		return unnext - nex;
	}
	proto_base() {
		instanc;
		fields;
		//fields.reserve(20);
	}
protected:
	unsigned char *next;
	unsigned char *end;
	const unsigned char *unnext;
	const unsigned char *unend;
	size_t _size;
	bool _is_default;
	//vector<operater> fields;
	//public:
	static vector<operater> fields;
	static T instanc;
	static bool inited;
};
template<class T>
T proto_base<T>::instanc;
template<class T>
vector<typename proto_base<T>::operater> proto_base<T>::fields;
template<class T>
bool proto_base<T>::inited = false;



#endif //  SPEED_UNLIMITE