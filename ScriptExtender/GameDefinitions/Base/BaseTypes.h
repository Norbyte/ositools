#pragma once

#include <array>
#include <string>
#include <cassert>
#include <optional>
#include <atomic>
#include <glm/glm.hpp>

#include <GameDefinitions/Base/BaseUtilities.h>

BEGIN_SE()

using Vector3 = glm::vec3;

struct NetId
{
	static constexpr uint32_t Unassigned = 0xffffffff;

	uint32_t Id;

	inline constexpr NetId() : Id(Unassigned) {}
	inline constexpr NetId(uint32_t id) : Id(id) {}
	inline constexpr NetId(NetId const & id) : Id(id.Id) {}

	inline explicit operator bool() const
	{
		return Id != Unassigned;
	}

	inline bool operator !() const
	{
		return Id == Unassigned;
	}

	inline constexpr uint16_t GetIndex() const
	{
		return (uint16_t)Id;
	}

	inline constexpr uint16_t GetSalt() const
	{
		return (uint16_t)(Id >> 16);
	}
};

constexpr NetId NetIdUnassigned{ 0xffffffff };

using PeerId = int32_t;

struct UserId
{
	static constexpr int32_t Unassigned = (int32_t)0xFFFF0000;

	int32_t Id;

	inline constexpr UserId() : Id(Unassigned) {}
	inline explicit constexpr UserId(int32_t id) : Id(id) {}
	inline constexpr UserId(UserId const& id) : Id(id.Id) {}

	inline operator bool() const
	{
		return Id != Unassigned;
	}

	inline bool operator !() const
	{
		return Id == Unassigned;
	}

	inline bool operator ==(UserId const& o) const
	{
		return Id == o.Id;
	}

	inline bool operator !=(UserId const& o) const
	{
		return Id != o.Id;
	}

	inline constexpr PeerId GetPeerId() const
	{
		return (PeerId)(Id >> 16);
	}
};

static constexpr UserId ReservedUserId{ UserId::Unassigned };



template <class T>
struct TypedHandle
{
	static constexpr uint64_t NullHandle = 0ull;

	uint64_t Handle;

	inline TypedHandle()
		: Handle(NullHandle)
	{}

	explicit inline TypedHandle(uint64_t handle)
		: Handle(handle)
	{}

	explicit inline TypedHandle(int64_t handle)
		: Handle((uint64_t)handle)
	{}

	inline TypedHandle(uint64_t type, uint64_t index, uint64_t salt)
	{
		assert(type < 0x400 && salt < 0x400000);
		Handle = index | (salt << 32) | (type << 54);
	}

	inline TypedHandle(TypedHandle const & oh)
		: Handle(oh.Handle)
	{}

	inline TypedHandle& operator = (TypedHandle const & oh)
	{
		Handle = oh.Handle;
		return *this;
	}

	inline bool operator == (TypedHandle const & oh) const
	{
		return Handle == oh.Handle;
	}

	inline uint32_t GetType() const
	{
		return Handle >> 54;
	}

	inline uint32_t GetSalt() const
	{
		return (Handle >> 32) & 0x3fffff;
	}

	inline uint32_t GetIndex() const
	{
		return (uint32_t)(Handle & 0xffffffff);
	}

	explicit inline operator bool() const
	{
		return Handle != NullHandle;
	}

	inline bool operator !() const
	{
		return Handle == NullHandle;
	}

	explicit inline operator int64_t() const
	{
		return (int64_t)Handle;
	}
};

enum EntityHandleTag {};
enum GenericComponentHandleTag {};

using EntityHandle = TypedHandle<EntityHandleTag>;
using ComponentHandle = TypedHandle<GenericComponentHandleTag>;

template <class T>
inline uint64_t Hash(TypedHandle<T> const& h)
{
	return h.Handle;
}

struct ComponentHandleWithType
{
	int64_t TypeId;
	ComponentHandle Handle;

	inline bool IsValid() const
	{
		return TypeId != -1 && !!Handle;
	}
};

struct Pool
{
	int PoolSize;
	int MaxSize;
#if !defined(OSI_EOCAPP)
	char* PoolName;
#endif
};

struct GameTime
{
	double Time;
	float DeltaTime;
	int32_t Ticks;
};

END_SE()


namespace std
{
	template<> struct hash<dse::EntityHandle>
	{
		typedef dse::EntityHandle argument_type;
		typedef std::size_t result_type;

		result_type operator()(argument_type const& fn) const noexcept
		{
			return std::hash<uint64_t>{}(fn.Handle);
		}
	};

	template<> struct hash<dse::ComponentHandle>
	{
		typedef dse::ComponentHandle argument_type;
		typedef std::size_t result_type;

		result_type operator()(argument_type const& fn) const noexcept
		{
			return std::hash<uint64_t>{}(fn.Handle);
		}
	};

	template<> struct hash<dse::UserId>
	{
		typedef dse::UserId argument_type;
		typedef std::size_t result_type;

		result_type operator()(argument_type const& fn) const noexcept
		{
			return std::hash<int32_t>{}(fn.Id);
		}
	};
}
