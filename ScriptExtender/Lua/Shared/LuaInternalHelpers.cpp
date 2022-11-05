#pragma once

#include <stdafx.h>
#include <Lua/Shared/LuaHelpers.h>
#include <Lua/Shared/LuaLifetime.h>
#include <Lua/Shared/LuaBinding.h>
#include <Extender/ScriptExtender.h>

#include <cstdint>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <lobject.h>
#include <lstate.h>
#include <lstring.h>
#include <lapi.h>

BEGIN_NS(lua)

struct LuaInternalState
{
	TValue canonicalizationCache;
};

Lifetime* LifetimeHandle::GetLifetime(lua_State* L) const
{
	auto& pool = State::FromLua(L)->GetLifetimePool();
	return pool.Get(*this);
}

LifetimeStackPin::LifetimeStackPin(lua_State* L)
	: LifetimeStackPin(State::FromLua(L)->GetStack())
{}

StaticLifetimeStackPin::StaticLifetimeStackPin(lua_State * L, LifetimeHandle lifetime)
	: StaticLifetimeStackPin(State::FromLua(L)->GetStack(), lifetime)
{}

LifetimeOwnerPin::LifetimeOwnerPin(lua_State* L)
	: LifetimeOwnerPin(State::FromLua(L)->GetLifetimePool())
{}

LifetimeOwnerPin::LifetimeOwnerPin(lua_State* L, LifetimeHandle const& lifetime)
	: LifetimeOwnerPin(State::FromLua(L)->GetLifetimePool(), lifetime)
{}


TValue* lua_index2addr(lua_State* L, int idx)
{
	CallInfo* ci = L->ci;
	if (idx > 0 && idx <= L->top - (ci->func + 1)) {
		TValue* o = ci->func + idx;
		if (o >= L->top) {
			luaL_error(L, "Invalid stack index: %d", idx);
			return nullptr;
		} else {
			return o;
		}
	} else if (idx >= 0) {
		luaL_error(L, "Param #%d missing", idx);
		return nullptr;
	} else {
		luaL_error(L, "Non-absolute stack indices not supported: %d", idx);
		return nullptr;
	}
}
	
TValue* lua_get_top_addr(lua_State* L)
{
	return L->top - 1;
}
	
Table* lua_get_top_table_unsafe(lua_State* L)
{
	return hvalue(L->top - 1);
}

Table* lua_get_array(lua_State* L, int idx)
{
	auto val = lua_index2addr(L, idx);
	if (!ttistable(val)) {
		luaL_error(L, "Param %d: expected a table", idx);
		return nullptr;
	}

	auto tab = hvalue(val);
	if (tab->lsizenode > 0) {
		luaL_error(L, "Param %d must be an array-like table, not a hashtable", idx);
		return nullptr;
	}

	return tab;
}

unsigned int lua_get_array_size(Table* arr)
{
	unsigned int i;
	for (i = arr->sizearray; i > 0 && ttisnil(&arr->array[i - 1]); i--) {}
	return i;
}

Table* lua_get_array_n(lua_State* L, int idx, int size)
{
	auto arr = lua_get_array(L, idx);
	auto arrSize = lua_get_array_size(arr);
	if (arrSize != size) {
		luaL_error(L, "Param %d: expected %d-element array, got a %d-element array", idx, size, arrSize);
		return nullptr;
	}

	return arr;
}

float lua_val_get_float(lua_State* L, TValue* val)
{
	if (ttisfloat(val)) {
		return (float)fltvalue(val);
	} else if (ttisinteger(val)) {
		return (float)ivalue(val);
	} else {
		luaL_error(L, "Expected number value, got %s", lua_typename(L, ttnov(val)));
		return 0.0f;
	}
}

glm::vec2 get_raw(lua_State* L, Table* arr, Overload<glm::vec2>)
{
	return glm::vec2{
		lua_val_get_float(L, arr->array),
		lua_val_get_float(L, arr->array + 1),
	};
}

glm::vec3 get_raw(lua_State* L, Table* arr, Overload<glm::vec3>)
{
	return glm::vec3{
		lua_val_get_float(L, arr->array),
		lua_val_get_float(L, arr->array + 1),
		lua_val_get_float(L, arr->array + 2),
	};
}

glm::vec4 get_raw(lua_State* L, Table* arr, Overload<glm::vec4>)
{
	return glm::vec4{
		lua_val_get_float(L, arr->array),
		lua_val_get_float(L, arr->array + 1),
		lua_val_get_float(L, arr->array + 2),
		lua_val_get_float(L, arr->array + 3),
	};
}

glm::quat get_raw(lua_State* L, Table* arr, Overload<glm::quat>)
{
	// quat constructor uses W,X,Y,Z
	return glm::quat{
		lua_val_get_float(L, arr->array + 3),
		lua_val_get_float(L, arr->array),
		lua_val_get_float(L, arr->array + 1),
		lua_val_get_float(L, arr->array + 2),
	};
}

glm::mat3 get_raw(lua_State* L, Table* arr, Overload<glm::mat3>)
{
	return glm::mat3{
		lua_val_get_float(L, arr->array),
		lua_val_get_float(L, arr->array + 1),
		lua_val_get_float(L, arr->array + 2),
		lua_val_get_float(L, arr->array + 3),
		lua_val_get_float(L, arr->array + 4),
		lua_val_get_float(L, arr->array + 5),
		lua_val_get_float(L, arr->array + 6),
		lua_val_get_float(L, arr->array + 7),
		lua_val_get_float(L, arr->array + 8),
	};
}

glm::mat4 get_raw(lua_State* L, Table* arr, Overload<glm::mat4>)
{
	return glm::mat4{
		lua_val_get_float(L, arr->array),
		lua_val_get_float(L, arr->array + 1),
		lua_val_get_float(L, arr->array + 2),
		lua_val_get_float(L, arr->array + 3),
		lua_val_get_float(L, arr->array + 4),
		lua_val_get_float(L, arr->array + 5),
		lua_val_get_float(L, arr->array + 6),
		lua_val_get_float(L, arr->array + 7),
		lua_val_get_float(L, arr->array + 8),
		lua_val_get_float(L, arr->array + 9),
		lua_val_get_float(L, arr->array + 10),
		lua_val_get_float(L, arr->array + 11),
		lua_val_get_float(L, arr->array + 12),
		lua_val_get_float(L, arr->array + 13),
		lua_val_get_float(L, arr->array + 14),
		lua_val_get_float(L, arr->array + 15),
	};
}

void set_raw(Table* tab, glm::vec2 const& v)
{
	setfltvalue(tab->array + 0, v.x);
	setfltvalue(tab->array + 1, v.y);
}

void set_raw(Table* tab, glm::vec3 const& v)
{
	setfltvalue(tab->array + 0, v.x);
	setfltvalue(tab->array + 1, v.y);
	setfltvalue(tab->array + 2, v.z);
}

void set_raw(Table* tab, glm::vec4 const& v)
{
	setfltvalue(tab->array + 0, v.x);
	setfltvalue(tab->array + 1, v.y);
	setfltvalue(tab->array + 2, v.z);
	setfltvalue(tab->array + 3, v.w);
}

void set_raw(Table* tab, glm::quat const& v)
{
	setfltvalue(tab->array + 0, v.x);
	setfltvalue(tab->array + 1, v.y);
	setfltvalue(tab->array + 2, v.z);
	setfltvalue(tab->array + 3, v.w);
}

void set_raw(Table* tab, glm::mat3 const& m)
{
	setfltvalue(tab->array + 0, m[0].x);
	setfltvalue(tab->array + 1, m[0].y);
	setfltvalue(tab->array + 2, m[0].z);
	setfltvalue(tab->array + 3, m[1].x);
	setfltvalue(tab->array + 4, m[1].y);
	setfltvalue(tab->array + 5, m[1].z);
	setfltvalue(tab->array + 6, m[2].x);
	setfltvalue(tab->array + 7, m[2].y);
	setfltvalue(tab->array + 8, m[2].z);
}

void set_raw(Table* tab, glm::mat4 const& m)
{
	setfltvalue(tab->array + 0, m[0].x);
	setfltvalue(tab->array + 1, m[0].y);
	setfltvalue(tab->array + 2, m[0].z);
	setfltvalue(tab->array + 3, m[0].w);
	setfltvalue(tab->array + 4, m[1].x);
	setfltvalue(tab->array + 5, m[1].y);
	setfltvalue(tab->array + 6, m[1].z);
	setfltvalue(tab->array + 7, m[1].w);
	setfltvalue(tab->array + 8, m[2].x);
	setfltvalue(tab->array + 9, m[2].y);
	setfltvalue(tab->array + 10, m[2].z);
	setfltvalue(tab->array + 11, m[2].w);
	setfltvalue(tab->array + 12, m[3].x);
	setfltvalue(tab->array + 13, m[3].y);
	setfltvalue(tab->array + 14, m[3].z);
	setfltvalue(tab->array + 15, m[3].w);
}

glm::vec2 do_get(lua_State* L, int index, Overload<glm::vec2>)
{
	auto i = lua_absindex(L, index);
	auto arr = lua_get_array_n(L, i, 2);
	return get_raw(L, arr, Overload<glm::vec2>{});
}

glm::vec3 do_get(lua_State* L, int index, Overload<glm::vec3>)
{
	auto i = lua_absindex(L, index);
	auto arr = lua_get_array_n(L, i, 3);
	return get_raw(L, arr, Overload<glm::vec3>{});
}

glm::vec4 do_get(lua_State* L, int index, Overload<glm::vec4>)
{
	auto i = lua_absindex(L, index);
	auto arr = lua_get_array_n(L, i, 4);
	return get_raw(L, arr, Overload<glm::vec4>{});
}

glm::quat do_get(lua_State* L, int index, Overload<glm::quat>)
{
	auto i = lua_absindex(L, index);
	auto arr = lua_get_array_n(L, i, 4);
	return get_raw(L, arr, Overload<glm::quat>{});
}

glm::mat3 do_get(lua_State* L, int index, Overload<glm::mat3>)
{
	auto i = lua_absindex(L, index);
	auto arr = lua_get_array_n(L, i, 9);
	return get_raw(L, arr, Overload<glm::mat3>{});
}

glm::mat4 do_get(lua_State* L, int index, Overload<glm::mat4>)
{
	auto i = lua_absindex(L, index);
	auto arr = lua_get_array_n(L, i, 16);
	return get_raw(L, arr, Overload<glm::mat4>{});
}

MathParam do_get(lua_State* L, int index, Overload<MathParam>)
{
	auto i = lua_absindex(L, index);
	auto arg = lua_index2addr(L, i);
	MathParam val;
	if (ttisnumber(arg)) {
		val.f = lua_val_get_float(L, arg);
		val.Arity = 1;
	} else if (ttistable(arg)) {
		auto tab = hvalue(arg);
		if (tab->lsizenode > 0) {
			luaL_error(L, "Param %d must be an array-like table, not a hashtable", index);
		}

		val.Arity = lua_get_array_size(tab);
		switch (val.Arity) {
		case 3: val.vec3 = get_raw(L, tab, Overload<glm::vec3>{}); break;
		case 4: val.vec4 = get_raw(L, tab, Overload<glm::vec4>{}); break;
		case 9: val.mat3 = get_raw(L, tab, Overload<glm::mat3>{}); break;
		case 16: val.mat4 = get_raw(L, tab, Overload<glm::mat4>{}); break;
		default: luaL_error(L, "Param %d: Unsupported vector or matrix size (%d)", index, val.Arity); break;
		}
	} else {
		luaL_error(L, "Param %d: expected a table", index);
		val.Arity = 0;
	}

	return val;
}

void push(lua_State* L, glm::vec2 const& v)
{
	lua_createtable(L, 2, 0);
	auto tab = lua_get_top_table_unsafe(L);
	set_raw(tab, v);
}

void push(lua_State* L, glm::vec3 const& v)
{
	lua_createtable(L, 3, 0);
	auto tab = lua_get_top_table_unsafe(L);
	set_raw(tab, v);
}

void push(lua_State* L, glm::vec4 const& v)
{
	lua_createtable(L, 4, 0);
	auto tab = lua_get_top_table_unsafe(L);
	set_raw(tab, v);
}

void push(lua_State* L, glm::quat const& v)
{
	lua_createtable(L, 4, 0);
	auto tab = lua_get_top_table_unsafe(L);
	set_raw(tab, v);
}

void push(lua_State* L, glm::mat3 const& m)
{
	lua_createtable(L, 9, 0);
	auto tab = lua_get_top_table_unsafe(L);
	set_raw(tab, m);
}

void push(lua_State* L, glm::mat4 const& m)
{
	lua_createtable(L, 16, 0);
	auto tab = lua_get_top_table_unsafe(L);
	set_raw(tab, m);
}

void assign(lua_State* L, int idx, glm::vec2 const& v)
{
	auto tab = lua_get_array_n(L, idx, 2);
	set_raw(tab, v);
}

void assign(lua_State* L, int idx, glm::vec3 const& v)
{
	auto tab = lua_get_array_n(L, idx, 3);
	set_raw(tab, v);
}

void assign(lua_State* L, int idx, glm::vec4 const& v)
{
	auto tab = lua_get_array_n(L, idx, 4);
	set_raw(tab, v);
}

void assign(lua_State* L, int idx, glm::quat const& v)
{
	auto tab = lua_get_array_n(L, idx, 4);
	set_raw(tab, v);
}

void assign(lua_State* L, int idx, glm::mat3 const& m)
{
	auto tab = lua_get_array_n(L, idx, 9);
	set_raw(tab, m);
}

void assign(lua_State* L, int idx, glm::mat4 const& m)
{
	auto tab = lua_get_array_n(L, idx, 16);
	set_raw(tab, m);
}

struct CppObjectUdata
{
	uint64_t PtrAndPropertyMapTag;
	uint64_t LifetimeAndTypeTag;
};


// Lua C++ objects store an additional lifetime in the Lua value; however, for optimization purposes
// only the lower 48 bits of the pointer are stored.
// (Even though pointers in x64 are 64-bit, it only actually uses 48 bits; upper 16 bits are reserved.)
// HOWEVER, this means that this storage format should not be used for values that use all 64 bits
// (namely, bitfields).
struct CppPointerVal
{
	// HW pointer size on x64 hardware; upper 16 bits always 0
	static constexpr unsigned PointerBits = 48;
	static constexpr unsigned PropertyMapTagBits = sizeof(std::uintptr_t)*8 - PointerBits;
	static constexpr std::uintptr_t PointerMask = (1ull << PointerBits) - 1;
	static constexpr std::uintptr_t PropertyMapTagMask = (1ull << PropertyMapTagBits) - 1;
	// First byte of extra data reserved for Lua type tag (tt_)
	static constexpr unsigned TypeTagBits = 8;
	static constexpr std::uintptr_t TypeTagMask = (1ull << TypeTagBits) - 1;
	static_assert(LifetimeHandle::HandleBits + TypeTagBits <= 56);

	CppPointerVal(TValue* val)
		: value_(val)
	{}
	
	CppPointerVal(lua_State* L, int idx)
		: value_(lua_index2addr(L, idx))
	{
		if (!IsCppObject(value_)) {
			luaL_error(L, "Param %d must be a C++ object", idx);
		}
	}

	static inline bool IsCppObject(TValue* val)
	{
		return ttislightcppobject(val) || ttiscppobject(val);
	}

	static inline void* PtrFromRaw(uint64_t rawValue)
	{
		return (void*)(rawValue & PointerMask);
	}

	static inline uint16_t PropertyMapTagFromRaw(uint64_t rawValue)
	{
		return (uint16_t)((rawValue >> PointerBits) & PropertyMapTagMask);
	}

	static inline LifetimeHandle LifetimeFromExtra(uint64_t extra)
	{
		return LifetimeHandle((extra >> TypeTagBits) & LifetimeHandle::HandleMask);
	}

	static inline MetatableTag MetatableTagFromExtra(uint64_t extra)
	{
		return (lua::MetatableTag)(extra & TypeTagMask);
	}

	static inline uint64_t MakeRaw(void* ptr, uint16_t propertyMapTag)
	{
		return ((uint64_t)ptr & PointerMask) | (((uint64_t)propertyMapTag & PropertyMapTagMask) << PointerBits);
	}

	static inline uint64_t MakeExtra(MetatableTag metatableTag, LifetimeHandle const& lifetime)
	{
		return (((uint64_t)lifetime.handle_ & LifetimeHandle::HandleMask) << TypeTagBits)
			| ((uint64_t)metatableTag & TypeTagMask);
	}

	inline uint64_t RawValue() const
	{
		return lcppvalue(value_);
	}

	inline uint64_t Extra() const
	{
		return valextra(value_);
	}

	inline CppObjectUdata* GetUdata() const
	{
		assert(ttiscppobject(value_));
		auto val = cppvalue(value_);
		return (CppObjectUdata *)getcppmem(val);
	}

	inline void* Ptr() const
	{
		if (ttislightcppobject(value_)) {
			return PtrFromRaw(RawValue());
		} else {
			auto udata = GetUdata();
			return PtrFromRaw(udata->PtrAndPropertyMapTag);
		}
	}

	inline uint16_t PropertyMapTag() const
	{
		if (ttislightcppobject(value_)) {
			return PropertyMapTagFromRaw(RawValue());
		} else {
			auto udata = GetUdata();
			return PropertyMapTagFromRaw(udata->PtrAndPropertyMapTag);
		}
	}

	inline LifetimeHandle Lifetime() const
	{
		if (ttislightcppobject(value_)) {
			return LifetimeFromExtra(Extra());
		} else {
			auto udata = GetUdata();
			return LifetimeFromExtra(udata->LifetimeAndTypeTag);
		}
	}

	inline MetatableTag MetatableTag() const
	{
		if (ttislightcppobject(value_)) {
			return MetatableTagFromExtra(Extra());
		} else {
			auto udata = GetUdata();
			return MetatableTagFromExtra(udata->LifetimeAndTypeTag);
		}
	}

	TValue* value_;
};


// Light C++ object storage format that keeps all 64-bits of the value;
// the property map tag is moved to the "extra" word, and no lifetime is kept.
struct CppValue
{
	// NOTE: TypeTagOffset must match offset in CppPointerVal!
	static constexpr unsigned TypeTagOffset = 0;
	static constexpr unsigned TypeTagBits = 8;
	static constexpr std::uintptr_t TypeTagMask = (1ull << TypeTagBits) - 1;
	static constexpr unsigned PropertyMapTagOffset = TypeTagBits;
	static constexpr unsigned PropertyMapTagBits = sizeof(std::uintptr_t) * 8 - TypeTagBits - 8 /* Lua tt_ */;
	static constexpr std::uintptr_t PropertyMapTagMask = (1ull << PropertyMapTagBits) - 1;

	CppValue(TValue* val)
		: value_(val)
	{}
	
	CppValue(lua_State* L, int idx)
		: value_(lua_index2addr(L, idx))
	{
		if (!IsCppObject(value_)) {
			luaL_error(L, "Param %d must be a C++ object", idx);
		}
	}

	static inline bool IsCppObject(TValue* val)
	{
		return ttislightcppobject(val);
	}

	static inline MetatableTag MetatableTagFromExtra(uint64_t extra)
	{
		return (lua::MetatableTag)((extra >> TypeTagOffset) & TypeTagMask);
	}

	static inline uint32_t PropertyMapTagFromExtra(uint64_t extra)
	{
		return (uint32_t)((extra >> PropertyMapTagOffset) & PropertyMapTagMask);
	}

	static inline uint64_t MakeRaw(uint64_t value)
	{
		return value;
	}

	static inline uint64_t MakeExtra(MetatableTag metatableTag, uint32_t propertyMapTag)
	{
		return (((uint64_t)propertyMapTag & PropertyMapTagMask) << PropertyMapTagOffset) 
			 | (((uint64_t)metatableTag & TypeTagMask) << TypeTagOffset);
	}

	inline uint64_t RawValue() const
	{
		return lcppvalue(value_);
	}

	inline uint64_t Extra() const
	{
		return valextra(value_);
	}

	inline CppObjectUdata* GetUdata() const
	{
		assert(ttiscppobject(value_));
		auto val = cppvalue(value_);
		return (CppObjectUdata *)getcppmem(val);
	}

	inline uint64_t Value() const
	{
		return RawValue();
	}

	inline uint16_t PropertyMapTag() const
	{
		return PropertyMapTagFromExtra(Extra());
	}

	inline MetatableTag MetatableTag() const
	{
		return MetatableTagFromExtra(Extra());
	}

	TValue* value_;
};


void lua_push_cppobject(lua_State* L, MetatableTag metatableTag, int propertyMapIndex, void* object, LifetimeHandle const& lifetime)
{
	assert(propertyMapIndex >= 0);
	auto val = CppPointerVal::MakeRaw(object, propertyMapIndex);
	auto extra = CppPointerVal::MakeExtra(metatableTag, lifetime);
	lua_pushlightcppobject(L, val, extra);
}

void lua_get_cppobject(lua_State* L, int idx, MetatableTag expectedMetatableTag, CppObjectMetadata& obj)
{
	CppPointerVal val(L, idx);
	auto metatableTag = val.MetatableTag();
	if (metatableTag != expectedMetatableTag) {
		luaL_error(L, "Param %d must be a C++ of type %d; got %d", idx, expectedMetatableTag, metatableTag);
	}

	obj.Ptr = val.Ptr();
	obj.MetatableTag = metatableTag;
	obj.PropertyMapTag = val.PropertyMapTag();
	obj.Lifetime = val.Lifetime();
}

void lua_get_cppobject(lua_State* L, int idx, CppObjectMetadata& obj)
{
	CppPointerVal val(L, idx);
	obj.Ptr = val.Ptr();
	obj.MetatableTag = val.MetatableTag();
	obj.PropertyMapTag = val.PropertyMapTag();
	obj.Lifetime = val.Lifetime();
}

bool lua_try_get_cppobject(lua_State* L, int idx, MetatableTag expectedTypeTag, CppObjectMetadata& obj)
{
	auto value = lua_index2addr(L, idx);
	if (!CppPointerVal::IsCppObject(value)) {
		return false;
	}

	CppPointerVal val(value);
	auto metatableTag = val.MetatableTag();
	if (metatableTag != expectedTypeTag) {
		return false;
	}

	obj.Ptr = val.Ptr();
	obj.MetatableTag = metatableTag;
	obj.PropertyMapTag = val.PropertyMapTag();
	obj.Lifetime = val.Lifetime();
	return true;
}

bool lua_try_get_cppobject(lua_State* L, int idx, CppObjectMetadata& obj)
{
	auto value = lua_index2addr(L, idx);
	if (!CppPointerVal::IsCppObject(value)) {
		return false;
	}

	CppPointerVal val(value);
	obj.Ptr = val.Ptr();
	obj.MetatableTag = val.MetatableTag();
	obj.PropertyMapTag = val.PropertyMapTag();
	obj.Lifetime = val.Lifetime();
	return true;
}



void lua_push_cppvalue(lua_State* L, MetatableTag metatableTag, int propertyMapIndex, uint64_t object)
{
	assert(propertyMapIndex >= 0);
	auto val = CppValue::MakeRaw(object);
	auto extra = CppValue::MakeExtra(metatableTag, propertyMapIndex);
	lua_pushlightcppobject(L, val, extra);
}

void lua_get_cppvalue(lua_State* L, int idx, MetatableTag expectedMetatableTag, CppValueMetadata& obj)
{
	CppValue val(L, idx);
	auto metatableTag = val.MetatableTag();
	if (metatableTag != expectedMetatableTag) {
		luaL_error(L, "Param %d must be a C++ of type %d; got %d", idx, expectedMetatableTag, metatableTag);
	}

	obj.Value = val.Value();
	obj.MetatableTag = metatableTag;
	obj.PropertyMapTag = val.PropertyMapTag();
}

void lua_get_cppvalue(lua_State* L, int idx, CppValueMetadata& obj)
{
	CppValue val(L, idx);
	obj.Value = val.Value();
	obj.MetatableTag = val.MetatableTag();
	obj.PropertyMapTag = val.PropertyMapTag();
}

void lua_get_cppvalue(lua_State* L, TValue* v, CppValueMetadata& obj)
{
	CppValue val(v);
	obj.Value = val.Value();
	obj.MetatableTag = val.MetatableTag();
	obj.PropertyMapTag = val.PropertyMapTag();
}

bool lua_try_get_cppvalue(lua_State* L, int idx, MetatableTag expectedTypeTag, CppValueMetadata& obj)
{
	auto value = lua_index2addr(L, idx);
	if (!CppValue::IsCppObject(value)) {
		return false;
	}

	CppValue val(value);
	auto metatableTag = val.MetatableTag();
	if (metatableTag != expectedTypeTag) {
		return false;
	}

	obj.Value = val.Value();
	obj.MetatableTag = metatableTag;
	obj.PropertyMapTag = val.PropertyMapTag();
	return true;
}

bool lua_try_get_cppvalue(lua_State* L, int idx, CppValueMetadata& obj)
{
	auto value = lua_index2addr(L, idx);
	if (!CppValue::IsCppObject(value)) {
		return false;
	}

	CppValue val(value);
	obj.Value = val.Value();
	obj.MetatableTag = val.MetatableTag();
	obj.PropertyMapTag = val.PropertyMapTag();
	return true;
}



void* LuaCppAlloc(lua_State* L, size_t size)
{
	throw std::runtime_error("Unsupported!");
}

void LuaCppFree(lua_State* L, void* block, size_t size)
{
	throw std::runtime_error("Unsupported!");
}

CMetatable* LuaCppGetLightMetatable(lua_State* L, unsigned long long val, unsigned long long extra)
{
	// Don't check lifetime here, as fetching metatable requires no access to the underlying object.
	// (Also the caller can cache the metamethod so checking here is useless)

	auto metatableTag = CppPointerVal::MetatableTagFromExtra(extra);
	return State::FromLua(L)->GetMetatableManager().GetMetatable(metatableTag);
}

CMetatable* LuaCppGetMetatable(lua_State* L, void* val, unsigned long long extra)
{
	throw std::runtime_error("Unsupported!");
}

LuaInternalState* lua_new_internal_state()
{
	auto st = GameAlloc<LuaInternalState>();
	setnilvalue(&st->canonicalizationCache);
	return st;
}

void lua_release_internal_state(LuaInternalState* state)
{
	GameDelete(state);
}

void* LuaCppCanonicalize(lua_State* L, void* val)
{
	auto v = reinterpret_cast<TValue*>(val);
	CppValueMetadata meta;
	lua_get_cppvalue(L, v, meta);
	if (meta.MetatableTag != MetatableTag::EnumValue) {
		return nullptr;
	}

	auto state = State::FromLua(L)->GetInternalState();
	auto label = EnumValueMetatable::GetLabel(meta);
	if (label) {
		TString* ts;
		ts = luaS_newlstr(L, label.GetString(), label.GetMetadata()->Length);
		setsvalue2s(L, &state->canonicalizationCache, ts);
	} else {
		setnilvalue(&state->canonicalizationCache);
	}

	return &state->canonicalizationCache;
}



FixedString do_get(lua_State* L, int index, Overload<FixedString>)
{
	StkId o = index2addr(L, index);
	if (ttisstring(o)) {
		auto s = tsvalue(o);
		auto & fs = *reinterpret_cast<FixedString *>(&s->cache);
		if (fs) {
			return fs;
		} else {
			return FixedString(getstr(s), tsslen(s));
		}
	}

	size_t len;
	auto str = luaL_tolstring(L, index, &len);
	auto fs = FixedString(str, len);
	lua_pop(L, 1);
	return fs;
}

void push(lua_State* L, FixedString const& v)
{
	lua_lock(L);
	TString* ts;
	if (v) {
		ts = luaS_new(L, v.GetString());
		auto fs = reinterpret_cast<FixedString*>(&ts->cache);
		new (fs) FixedString(v);
	} else {
		ts = luaS_new(L, "");
	}

	setsvalue2s(L, L->top, ts);
	api_incr_top(L);
	luaC_checkGC(L);
	lua_unlock(L);
}

void LuaCacheString(lua_State* L, TString* s)
{
	static_assert(sizeof(LUA_STRING_EXTRATYPE) == sizeof(FixedString));
	auto fs = reinterpret_cast<FixedString*>(&s->cache);
	new (fs) FixedString(getstr(s), tsslen(s), FixedString::DontCreate{});
}

void LuaReleaseString(lua_State* L, TString* s)
{
	static_assert(sizeof(LUA_STRING_EXTRATYPE) == sizeof(FixedString));
	auto fs = reinterpret_cast<FixedString*>(&s->cache);
	fs->~FixedString();
}



GenericPropertyMap& LuaGetPropertyMap(int propertyMapIndex)
{
	return *gExtender->GetPropertyMapManager().GetPropertyMap(propertyMapIndex);
}

END_NS()
