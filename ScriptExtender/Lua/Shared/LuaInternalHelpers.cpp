#pragma once

#include <stdafx.h>
#include <Lua/Shared/LuaHelpers.h>

#include <cstdint>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <lobject.h>
#include <lstate.h>

namespace dse::lua
{
	TValue* lua_index2addr(lua_State* L, int idx)
	{
		CallInfo* ci = L->ci;
		if (idx > 0 && idx <= ci->top - (ci->func + 1)) {
			TValue* o = ci->func + idx;
			if (o >= L->top) {
				luaL_error(L, "Invalid stack index: %d", idx);
				return nullptr;
			} else {
				return o;
			}
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
}
