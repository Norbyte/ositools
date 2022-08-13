#pragma once

#include <Lua/Shared/LuaBinding.h>
#include <Lua/Shared/Proxies/LuaEntityProxy.h>
#include <GameDefinitions/GameObjects/RootTemplates.h>

namespace dse::lua
{
	class LuaSerializer : Noncopyable<LuaSerializer>
	{
	public:
		lua_State* L;
		bool IsWriting;

		inline LuaSerializer(lua_State* l, bool isWriting)
			: L(l), IsWriting(isWriting)
		{}

		template <class T>
		LuaSerializer& Visit(T& v)
		{
			if (IsWriting) {
				StackCheck _(L, 1);
				push(L, v);
			} else {
				StackCheck _(L);
				v = do_get(L, -1, Overload<T>{});
			}

			return *this;
		}

		template <class T>
		void VisitProperty(char const* key, T& val)
		{
			StackCheck _(L);
			if (IsWriting) {
				*this << val;
				lua_setfield(L, -2, key);
			} else {
				lua_getfield(L, -1, key);
				*this << val;
				lua_pop(L, 1);
			}
		}

		template <class T>
		void VisitOptionalProperty(char const* key, T& val, T const& defaultValue)
		{
			StackCheck _(L);
			if (IsWriting) {
				*this << val;
				lua_setfield(L, -2, key);
			} else {
				lua_getfield(L, -1, key);
				if (lua_isnil(L, -1)) {
					val = defaultValue;
				} else {
					*this << val;
				}
				lua_pop(L, 1);
			}
		}

		inline void BeginObject()
		{
			if (IsWriting) {
				lua_newtable(L);
			} else {
				luaL_checktype(L, -1, LUA_TTABLE);
			}
		}

		inline void EndObject() {}
	};

	template <class T>
	int LuaWrite(lua_State* L, T const& val)
	{
		StackCheck _(L, 1);
		LuaSerializer serializer(L, true);
		serializer << const_cast<T&>(val);
		return 1;
	}

	template <class T>
	void LuaRead(lua_State* L, T& val)
	{
		StackCheck _(L);
		LuaSerializer serializer(L, false);
		serializer << val;
	}

	inline LuaSerializer& operator << (LuaSerializer& s, bool& v) { return s.Visit(v); }
	inline LuaSerializer& operator << (LuaSerializer& s, int8_t& v) { return s.Visit(v); }
	inline LuaSerializer& operator << (LuaSerializer& s, uint8_t& v) { return s.Visit(v); }
	inline LuaSerializer& operator << (LuaSerializer& s, int16_t& v) { return s.Visit(v); }
	inline LuaSerializer& operator << (LuaSerializer& s, uint16_t& v) { return s.Visit(v); }
	inline LuaSerializer& operator << (LuaSerializer& s, int32_t& v) { return s.Visit(v); }
	inline LuaSerializer& operator << (LuaSerializer& s, uint32_t& v) { return s.Visit(v); }
	inline LuaSerializer& operator << (LuaSerializer& s, int64_t& v) { return s.Visit(v); }
	inline LuaSerializer& operator << (LuaSerializer& s, uint64_t& v) { return s.Visit(v); }
	inline LuaSerializer& operator << (LuaSerializer& s, float& v) { return s.Visit(v); }
	inline LuaSerializer& operator << (LuaSerializer& s, double& v) { return s.Visit(v); }
	inline LuaSerializer& operator << (LuaSerializer& s, FixedString& v) { return s.Visit(v); }
	inline LuaSerializer& operator << (LuaSerializer& s, STDString& v) { return s.Visit(v); }
	inline LuaSerializer& operator << (LuaSerializer& s, STDWString& v) { return s.Visit(v); }
	inline LuaSerializer& operator << (LuaSerializer& s, Guid& v) { return s.Visit(v); }
	inline LuaSerializer& operator << (LuaSerializer& s, Path& v) { return s.Visit(v); }
	inline LuaSerializer& operator << (LuaSerializer& s, NetId& v) { return s.Visit(v); }
	inline LuaSerializer& operator << (LuaSerializer& s, UserId& v) { return s.Visit(v); }
	inline LuaSerializer& operator << (LuaSerializer& s, ComponentHandle& v) { return s.Visit(v); }
	inline LuaSerializer& operator << (LuaSerializer& s, EntityHandle& v) { return s.Visit(v); }
	inline LuaSerializer& operator << (LuaSerializer& s, TemplateHandle& v) { return s.Visit(v); }
	inline LuaSerializer& operator << (LuaSerializer& s, glm::ivec2& v) { return s.Visit(v); }
	inline LuaSerializer& operator << (LuaSerializer& s, glm::i16vec2& v) { return s.Visit(v); }
	inline LuaSerializer& operator << (LuaSerializer& s, glm::vec2& v) { return s.Visit(v); }
	inline LuaSerializer& operator << (LuaSerializer& s, glm::vec3& v) { return s.Visit(v); }
	inline LuaSerializer& operator << (LuaSerializer& s, glm::vec4& v) { return s.Visit(v); }
	inline LuaSerializer& operator << (LuaSerializer& s, glm::mat3& v) { return s.Visit(v); }
	inline LuaSerializer& operator << (LuaSerializer& s, glm::mat4& v) { return s.Visit(v); }
	inline LuaSerializer& operator << (LuaSerializer& s, Version& v) { return s.Visit(v); }
	LuaSerializer& operator << (LuaSerializer& s, TranslatedString& v);
	LuaSerializer& operator << (LuaSerializer& s, char const*& v);
	LuaSerializer& operator << (LuaSerializer& s, ig::InvokeDataValue& v);
	LuaSerializer& operator << (LuaSerializer& s, TypeInformationRef& v);

	template <class T>
	LuaSerializer& operator << (LuaSerializer& s, OverrideableProperty<T>& v)
	{
		s << v.Value;
		if (!s.IsWriting) {
			v.IsOverridden = true;
		}
		return s;
	}

	template <class T, class Allocator, bool StoreSize>
	LuaSerializer& operator << (LuaSerializer& s, ObjectSet<T, Allocator, StoreSize>& v)
	{
		s.BeginObject();
		if (s.IsWriting) {
			int i = 1;
			for (auto& val : v) {
				StackCheck _(s.L);
				push(s.L, i++);
				s << val;
				lua_settable(s.L, -3);
			}
		} else {
			v.clear();
			for (auto idx : iterate(s.L, -1)) {
				StackCheck _(s.L);
				T temp{};
				s << temp;
				v.push_back(temp);
			}
		}
		s.EndObject();
		return s;
	}

	template <class T>
	LuaSerializer& operator << (LuaSerializer& s, Array<T>& v)
	{
		s.BeginObject();
		if (s.IsWriting) {
			int i = 1;
			for (auto& val : v) {
				StackCheck _(s.L);
				push(s.L, i++);
				s << val;
				lua_settable(s.L, -3);
			}
		} else {
			v.Clear();
			for (auto idx : iterate(s.L, -1)) {
				StackCheck _(s.L);
				T temp{};
				s << temp;
				v.Add(temp);
			}
		}
		s.EndObject();
		return s;
	}

	template <class T>
	LuaSerializer& operator << (LuaSerializer& s, Vector<T>& v)
	{
		s.BeginObject();
		if (s.IsWriting) {
			int i = 1;
			for (auto& val : v) {
				StackCheck _(s.L);
				push(s.L, i++);
				s << val;
				lua_settable(s.L, -3);
			}
		} else {
			v.clear();
			for (auto idx : iterate(s.L, -1)) {
				StackCheck _(s.L);
				T temp{};
				s << temp;
				v.push_back(temp);
			}
		}
		s.EndObject();
		return s;
	}

	template <class TKey, class TValue>
	LuaSerializer& operator << (LuaSerializer& s, RefMap<TKey, TValue>& v)
	{
		s.BeginObject();
		if (s.IsWriting) {
			int i = 1;
			for (auto& it : v) {
				StackCheck _(s.L);
				s << it.Key << it.Value;
				lua_settable(s.L, -3);
			}
		} else {
			v.Clear();
			for (auto idx : iterate(s.L, -1)) {
				StackCheck _(s.L);
				TKey key{};
				TValue value{};
				lua_pushvalue(s.L, -2);
				s << key;
				lua_pop(s.L, 1);
				s << value;
				v.Insert(std::move(key), std::move(value));
			}
		}
		s.EndObject();
		return s;
	}

	template <class TKey, class TValue>
	LuaSerializer& operator << (LuaSerializer& s, Map<TKey, TValue>& v)
	{
		s.BeginObject();
		if (s.IsWriting) {
			int i = 1;
			for (auto& it : v) {
				StackCheck _(s.L);
				s << it.Key() << it.Value();
				lua_settable(s.L, -3);
			}
		} else {
			v.Clear();
			for (auto idx : iterate(s.L, -1)) {
				StackCheck _(s.L);
				TKey key{};
				TValue value{};
				lua_pushvalue(s.L, -2);
				s << key;
				lua_pop(s.L, 1);
				s << value;
				v.Insert(std::move(key), std::move(value));
			}
		}
		s.EndObject();
		return s;
	}

	template <class T>
	LuaSerializer& operator << (LuaSerializer& s, std::optional<T>& v)
	{
		if (s.IsWriting) {
			if (v) {
				s << *v;
			} else {
				push(s.L, nullptr);
			}
		} else {
			if (!v) {
				v = T();
			}

			s << *v;
		}

		return s;
	}

	template <class T>
	typename std::enable_if_t<std::is_pointer_v<T>, LuaSerializer&> operator << (LuaSerializer& s, T& v)
	{
		if (s.IsWriting) {
			if (v == nullptr) {
				lua_pushnil(s.L);
			} else {
				s << *v;
			}
		} else {
			if constexpr (decltype(IsAllocatable<std::remove_pointer_t<T>>(nullptr))::value) {
				if (v == nullptr) {
					v = GameAlloc<std::remove_pointer_t<T>>();
				}
			}

			if (v != nullptr) {
				s << *v;
			}
		}

		return s;
	}


	template <class T>
	typename std::enable_if_t<std::is_enum_v<T>, LuaSerializer&> operator << (LuaSerializer& s, T& v)
	{
		if constexpr (std::is_base_of_v<BitmaskInfoBase<T>, EnumInfo<T>>) {
			if (s.IsWriting) {
				push_flags(s.L, v);
			}
			else {
				v = checked_get_flags<T>(s.L, -1);
			}
		} else {
			static_assert(std::is_base_of_v<EnumInfoBase<T>, EnumInfo<T>>, "Cannot serialize an enumeration that has no EnumInfo!");
			if (s.IsWriting) {
				push(s.L, v);
			} else {
				v = get<T>(s.L, -1);
			}
		}

		return s;
	}

	void LuaSerializeStatsEnum(LuaSerializer& s, char const* key, FixedString const& enumName, int& v);

	LuaSerializer& operator << (LuaSerializer& s, stats::EquipmentSet& v);
	LuaSerializer& operator << (LuaSerializer& s, stats::EquipmentGroup& v);
	LuaSerializer& operator << (LuaSerializer& s, stats::SkillSet& v);
	LuaSerializer& operator << (LuaSerializer& s, stats::TreasureTable& v);
	LuaSerializer& operator << (LuaSerializer& s, stats::TreasureSubTableDescription& v);
	LuaSerializer& operator << (LuaSerializer& s, stats::TreasureSubTableDescription::Category& v);
	LuaSerializer& operator << (LuaSerializer& s, stats::TreasureSubTableDescription::DropCount& v);
	LuaSerializer& operator << (LuaSerializer& s, stats::TreasureCategory& v);
	LuaSerializer& operator << (LuaSerializer& s, stats::TreasureCategory::Item& v);
	LuaSerializer& operator << (LuaSerializer& s, stats::ItemCombination& v);
	LuaSerializer& operator << (LuaSerializer& s, stats::ItemCombinationIngredient& v);
	LuaSerializer& operator << (LuaSerializer& s, stats::ItemCombinationResult& v);
	LuaSerializer& operator << (LuaSerializer& s, stats::ItemCombinationResult::CombinationResult& v);
	LuaSerializer& operator << (LuaSerializer& s, stats::ItemCombinationPreviewData& v);
	LuaSerializer& operator << (LuaSerializer& s, stats::ItemCombinationProperty& v);
	LuaSerializer& operator << (LuaSerializer& s, stats::ItemCombinationProperty::Entry& v);
	LuaSerializer& operator << (LuaSerializer& s, stats::CItemGroup& v);
	LuaSerializer& operator << (LuaSerializer& s, stats::CLevelGroup& v);
	LuaSerializer& operator << (LuaSerializer& s, stats::CRootGroup& v);
	LuaSerializer& operator << (LuaSerializer& s, stats::CNameGroupLink& v);
	LuaSerializer& operator << (LuaSerializer& s, stats::NameGroup& v);
	LuaSerializer& operator << (LuaSerializer& s, stats::NameGroupName& v);
	LuaSerializer& operator << (LuaSerializer& s, stats::Requirement& v);
	LuaSerializer& operator << (LuaSerializer& s, stats::Requirement& v);
	LuaSerializer& operator << (LuaSerializer& s, stats::PropertyData& v);
	LuaSerializer& operator << (LuaSerializer& s, stats::PropertyCustom& v);
	LuaSerializer& operator << (LuaSerializer& s, stats::PropertyStatus& v);
	LuaSerializer& operator << (LuaSerializer& s, stats::PropertySurfaceChange& v);
	LuaSerializer& operator << (LuaSerializer& s, stats::PropertyGameAction& v);
	LuaSerializer& operator << (LuaSerializer& s, stats::PropertyOsirisTask& v);
	LuaSerializer& operator << (LuaSerializer& s, stats::PropertySabotage& v);
	LuaSerializer& operator << (LuaSerializer& s, stats::PropertySummon& v);
	LuaSerializer& operator << (LuaSerializer& s, stats::PropertyForce& v);
	LuaSerializer& operator << (LuaSerializer& s, stats::PropertyCustomDescription& v);
	LuaSerializer& operator << (LuaSerializer& s, stats::PropertyExtender& v);
	void SerializeObjectProperty(LuaSerializer& s, stats::PropertyData*& v);
	LuaSerializer& operator << (LuaSerializer& s, stats::PropertyList& v);
	LuaSerializer& operator << (LuaSerializer& s, stats::DeltaModifier& v);
	LuaSerializer& operator << (LuaSerializer& s, stats::ItemSet& v);
	LuaSerializer& operator << (LuaSerializer& s, stats::ItemColorDefinition& v);
	LuaSerializer& operator << (LuaSerializer& s, SurfaceTemplate::StatusData& v);
}
