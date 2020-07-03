#pragma once

// GENERATE_CODE 逆向生成其他语言代码
// SPEED_256 一个message限制字段256个，这种模式构造速度非常的高。
//           另外一种无限制的模式，构造速度差一些不过和google的protobuf接近
// USE_TAG //两种序列化格式，带Tag和不带Tag

//--------------------------------------------------------------------------
//#define GENERATE_CODE
//--------------------------------------------------------------------------

#ifdef GENERATE_CODE
	#include "collector.h"
#else
	//--------------------------------------------------------------------------
	//#define SPEED_256
	#define SPEED_UNLIMITE
	#define USE_TAG
    //--------------------------------------------------------------------------


	#define OP_SERIALIZE 1
	#define OP_UNSERIALIZE 2
	#define OP_SIZE 3
	#define OP_REFRESH 4
	#define OP_DEFAULT 5

	#include "fast_protobuf_unlimit.h"

	#ifdef USE_TAG
	#define OVERRIDE_OPERATER(ele,id,type) \
								public:\
								inline void operater_##id(uint8_t o){\
									switch (o)\
									{\
									case OP_SERIALIZE:{\
										if (!proto_field::is_default_(&ele)) {\
											serialize_tag_(id, type);\
											next += proto_field::serialize_(&ele, next, end);\
										}\
									}break;\
									case OP_UNSERIALIZE:{\
										if(type == _type)\
											unnext+=proto_field::unserialize_(&ele, unnext, unend);\
										else\
											scan_next();\
									}break;\
									case OP_SIZE:{_size+=(proto_field::size_(&ele)+size_tag_(id,type));}break;\
									case OP_REFRESH:{proto_field::refresh_(&ele);}break;\
									case OP_DEFAULT:{if(!proto_field::is_default_(&ele)){_is_default=false;return;}}break;\
											default:break;\
									}\
								}
	#else
	#define OVERRIDE_OPERATER(ele,id,type)\
								public:\
								inline void operater_##id(uint8_t o){\
									switch (o)\
									{\
									case OP_SERIALIZE:{next+=proto_field::serialize_(&ele, next, end);}break;\
									case OP_UNSERIALIZE:{unnext+=proto_field::unserialize_(&ele, unnext, unend);}break;\
									case OP_SIZE:{_size+=proto_field::size_(&ele);}break;\
									case OP_REFRESH:{proto_field::refresh_(&ele);}break;\
									case OP_DEFAULT:{if(!proto_field::is_default_(&ele)){_is_default=false;return;}}break;\
											default:break;\
									}\
								}
	#endif // USE_TAG
#endif // GENERATE_CODE

