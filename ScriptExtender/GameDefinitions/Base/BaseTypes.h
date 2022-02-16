#pragma once

#include <array>
#include <string>
#include <cassert>
#include <optional>
#include <atomic>
#include <glm/glm.hpp>

#include <GameDefinitions/Base/BaseUtilities.h>

BEGIN_SE()

struct NetId
{
	static constexpr uint32_t Unassigned = 0xffffffff;

	uint32_t Id;

	inline constexpr NetId() : Id(Unassigned) {}
	inline constexpr NetId(uint32_t id) : Id(id) {}
	inline constexpr NetId(NetId const & id) : Id(id.Id) {}

	inline bool operator !=(NetId const& o) const
	{
		return Id != o.Id;
	}

	inline bool operator ==(NetId const& o) const
	{
		return Id == o.Id;
	}

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

inline uint64_t Hash(NetId v)
{
	return v.Id;
}

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

inline uint64_t Hash(UserId v)
{
	return v.Id;
}



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

char const* GetHandleTypeName(ComponentHandle const& handle);

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

struct EntityHandleWithType
{
	int64_t TypeId;
	EntityHandle Handle;

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

struct Version
{
	inline Version() : Ver(0) {}
	inline Version(uint32_t ver) : Ver(ver) {}
	inline Version(uint32_t minor, uint32_t major, uint32_t revision, uint32_t build)
		: Ver(((major & 0xf) << 28) + ((minor & 0xf) << 24) + ((revision & 0xff) << 16) + (build & 0xffff))
	{}

	inline uint32_t Major() const
	{
		return Ver >> 28;
	}

	inline uint32_t Minor() const
	{
		return (Ver >> 24) & 0xf;
	}

	inline uint32_t Revision() const
	{
		return (Ver >> 16) & 0xff;
	}

	inline uint32_t Build() const
	{
		return Ver & 0xffff;
	}

	uint32_t Ver;
};

struct Transform
{
	glm::mat4 Matrix;
	glm::mat3 Rotate;
	glm::vec3 Translate;
	glm::vec3 Scale;
};

struct Bound
{
	glm::vec3 Center;
	float Radius;
	glm::vec3 Min;
	glm::vec3 Max;
	bool IsCenterSet;
};

using PlayerId = int16_t;

struct BitMatrix
{
	uint32_t* Matrix;
	int Width;
	int Height;

	inline bool Get(int x, int y)
	{
		auto offset = x + y * Width;
		return (this->Matrix[offset >> 5] & (1 << (offset & 0x1F))) != 0;
	}

	inline void Set(int x, int y)
	{
		auto offset = x + y * Width;
		this->Matrix[offset >> 5] |= (1 << (offset & 0x1F));
	}

	inline void Clear(int x, int y)
	{
		auto offset = x + y * Width;
		this->Matrix[offset >> 5] &= ~(1 << (offset & 0x1F));
	}
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
