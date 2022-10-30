#include <Lua/Shared/Proxies/LuaBitfieldValue.h>
#include <Extender/ScriptExtender.h>
#include <json/json.h>

BEGIN_NS(lua)

BitmaskInfoStore<EnumUnderlyingType>* BitfieldValueMetatable::GetBitfieldInfo(CppValueMetadata const& self)
{
	return BitmaskRegistry::Get().BitfieldsById[self.PropertyMapTag];
}

EnumUnderlyingType BitfieldValueMetatable::GetValue(CppValueMetadata const& self)
{
	return static_cast<EnumUnderlyingType>(self.Value);
}

Json::Value BitfieldValueMetatable::ToJson(CppValueMetadata& self)
{
	Json::Value arr(Json::arrayValue);
	auto ei = GetBitfieldInfo(self);
	for (auto const& val : ei->Values) {
		if ((self.Value & val.Value) == val.Value) {
			arr.append(val.Key.GetString());
		}
	}
	return arr;
}

int BitfieldValueMetatable::Index(lua_State* L, CppValueMetadata& self)
{
	auto index = get<FixedString>(L, 2);
	if (index == GFS.strLabels) {
		auto ei = GetBitfieldInfo(self);
		lua_newtable(L);
		int i = 1;
		for (auto const& val : ei->Values) {
			if ((self.Value & val.Value) == val.Value) {
				settable(L, i, val.Key, -3);
				i++;
			}
		}
	} else if (index == GFS.strValue) {
		push(L, GetValue(self));
	} else if (index == GFS.strEnumName) {
		push(L, GetBitfieldInfo(self)->EnumName);
	} else {
		luaL_error(L, "Bitfield values have no property named '%s'", index.GetStringOrDefault());
		push(L, nullptr);
	}

	return 1;
}

int BitfieldValueMetatable::ToString(lua_State* L, CppValueMetadata& self)
{
	StackCheck _(L, 1);
	STDString labels;
	auto ei = GetBitfieldInfo(self);
	for (auto const& val : ei->Values) {
		if ((self.Value & val.Value) == val.Value) {
			if (!labels.empty()) labels += ',';
			labels += val.Key.GetString();
		}
	}

	push(L, STDString(ei->EnumName.GetString()) + "(" + labels + ")");
	return 1;
}

bool BitfieldValueMetatable::IsEqual(lua_State* L, CppValueMetadata& self, int otherIndex)
{
	auto ei = GetBitfieldInfo(self);
	auto other = try_get_bitfield_value(L, otherIndex, *ei, false);
	return other && *other == self.Value;
}

int BitfieldValueMetatable::BAnd(lua_State* L, CppValueMetadata& self, int otherIndex)
{
	auto ei = GetBitfieldInfo(self);
	auto other = get_bitfield_value(L, otherIndex, *ei, true);
	Make(L, self.Value & other, ei->RegistryIndex);
	return 1;
}

int BitfieldValueMetatable::BOr(lua_State* L, CppValueMetadata& self, int otherIndex)
{
	auto ei = GetBitfieldInfo(self);
	auto other = get_bitfield_value(L, otherIndex, *ei, false);
	Make(L, self.Value | other, ei->RegistryIndex);
	return 1;
}

int BitfieldValueMetatable::BXor(lua_State* L, CppValueMetadata& self, int otherIndex)
{
	auto ei = GetBitfieldInfo(self);
	auto other = get_bitfield_value(L, otherIndex, *ei, false);
	Make(L, self.Value ^ other, ei->RegistryIndex);
	return 1;
}

int BitfieldValueMetatable::BNot(lua_State* L, CppValueMetadata& self)
{
	auto ei = GetBitfieldInfo(self);
	Make(L, (~self.Value & ei->AllowedFlags), ei->RegistryIndex);
	return 1;
}

char const* BitfieldValueMetatable::GetTypeName(lua_State* L, CppValueMetadata& self)
{
	auto ei = GetBitfieldInfo(self);
	return ei->EnumName.GetStringOrDefault();
}

EnumUnderlyingType get_bitfield_value(lua_State* L, int index, BitmaskInfoStore<EnumUnderlyingType> const& store, bool maskInvalidBits)
{
	switch (lua_type(L, index)) {
	case LUA_TSTRING:
	{
		auto val = do_get(L, index, Overload<FixedString>{});
		auto valueIndex = store.Find(val);
		if (valueIndex) {
			return *valueIndex;
		} else {
			luaL_error(L, "Param %d: not a valid '%s' bitfield value: %s", index, store.EnumName.GetStringOrDefault(), val.GetStringOrDefault());
		}
		break;
	}

	case LUA_TNUMBER:
	{
		auto val = (EnumUnderlyingType)lua_tointeger(L, index);
		if ((val & ~store.AllowedFlags) == 0) {
			return val;
		} else if (maskInvalidBits) {
			return (val & store.AllowedFlags);
		} else {
			luaL_error(L, "Param %d: not a valid '%s' bitfield value: %p", index, store.EnumName.GetStringOrDefault(), val);
		}
		break;
	}

	case LUA_TTABLE:
	{
		EnumUnderlyingType val{ 0 };
		for (auto valueIdx : iterate(L, index)) {
			auto label = do_get(L, valueIdx, Overload<FixedString>{});
			auto index = store.Find(label);
			if (index) {
				val |= *index;
			} else {
				luaL_error(L, "Param %d: not a valid '%s' bitfield value: %s", 
					index, store.EnumName.GetStringOrDefault(), label.GetString());
			}
		}

		return val;
	}

	case LUA_TLIGHTCPPOBJECT:
	{
		CppValueMetadata meta;
		lua_get_cppvalue(L, lua_absindex(L, index), meta);
		if (meta.MetatableTag == BitfieldValueMetatable::MetaTag && meta.PropertyMapTag == (unsigned)store.RegistryIndex) {
			return static_cast<EnumUnderlyingType>(meta.Value);
		} else {
			luaL_error(L, "Param %d: expected a '%s' bitfield value, got type %d", index, store.EnumName.GetStringOrDefault(), meta.MetatableTag);
		}
		break;
	}

	default:
		luaL_error(L, "Param %d: expected integer, string or table '%s' bitfield value, got %s", index,
			store.EnumName.GetStringOrDefault(), lua_typename(L, lua_type(L, index)));
		break;
	}

	return 0;
}

std::optional<EnumUnderlyingType> try_get_bitfield_value(lua_State* L, int index, BitmaskInfoStore<EnumUnderlyingType> const& store, bool maskInvalidBits)
{
	switch (lua_type(L, index)) {
	case LUA_TSTRING:
	{
		auto val = do_get(L, index, Overload<FixedString>{});
		auto valueIndex = store.Find(val);
		if (valueIndex) {
			return *valueIndex;
		}
		break;
	}

	case LUA_TNUMBER:
	{
		auto val = (EnumUnderlyingType)lua_tointeger(L, index);
		if ((val & ~store.AllowedFlags) == 0) {
			return val;
		} else if (maskInvalidBits) {
			return (val & store.AllowedFlags);
		}
		break;
	}

	case LUA_TTABLE:
	{
		EnumUnderlyingType val{ 0 };
		for (auto valueIdx : iterate(L, index)) {
			auto label = do_get(L, valueIdx, Overload<FixedString>{});
			auto index = store.Find(label);
			if (index) {
				val |= *index;
			} else {
				return {};
			}
		}

		return val;
	}

	case LUA_TLIGHTCPPOBJECT:
	{
		CppValueMetadata meta;
		lua_get_cppvalue(L, lua_absindex(L, index), meta);
		if (meta.MetatableTag == BitfieldValueMetatable::MetaTag && meta.PropertyMapTag == (unsigned)store.RegistryIndex) {
			return static_cast<EnumUnderlyingType>(meta.Value);
		}
		break;
	}
	}

	return {};
}

void push_bitfield_value(lua_State* L, EnumUnderlyingType value, BitmaskInfoStore<EnumUnderlyingType> const& store)
{
	BitfieldValueMetatable::Make(L, value, store.RegistryIndex);
}

END_NS()
