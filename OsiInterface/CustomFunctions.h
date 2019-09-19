#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

#include "Utils.h"
#include "OsiInterface.h"
#include "DivInterface.h"

namespace osidbg
{
	enum EoCFunctionType
	{
		Call = 1,
		Query = 2,
		Event = 3
	};

	struct FunctionHandle
	{
		inline constexpr FunctionHandle()
			: handle(0) {}

		inline constexpr FunctionHandle(uint32_t id)
			: handle(id) {}

		inline constexpr FunctionHandle(EoCFunctionType type, uint32_t classIndex, uint32_t functionIndex)
			: handle(((uint32_t)type & 7) | ((classIndex & 0x1ffff) << 3) | ((functionIndex & 0x3ff) << 20) | (1 << 30))
		{}

		inline constexpr EoCFunctionType type() const
		{
			return (EoCFunctionType)(handle & 7);
		}

		inline constexpr uint32_t classIndex() const
		{
			return (handle >> 3) & 0x1ffff;
		}

		inline constexpr uint32_t functionIndex() const
		{
			return (handle >> 20) & 0x3ff;
		}

		inline constexpr operator uint32_t() const
		{
			return handle;
		}

		inline constexpr bool operator == (FunctionHandle const & f) const
		{
			return handle == f.handle;
		}

		uint32_t handle;
	};

	struct FunctionNameAndArity
	{
		std::string Name;
		uint32_t Arity;

		inline bool operator == (FunctionNameAndArity const & fn) const
		{
			return Name == fn.Name && Arity == fn.Arity;
		}
	};
}

namespace std
{
	template<> struct hash<osidbg::FunctionNameAndArity>
	{
		typedef osidbg::FunctionNameAndArity argument_type;
		typedef std::size_t result_type;
		result_type operator()(argument_type const& fn) const noexcept
		{
			result_type const h1(std::hash<std::string>{}(fn.Name));
			result_type const h2(std::hash<uint32_t>{}(fn.Arity));
			return h1 ^ (h2 << 1);
		}
	};
}

namespace osidbg
{
	struct CustomFunctionParam
	{
		std::string Name;
		ValueType Type;
		FunctionArgumentDirection Dir;
	};

	class CustomFunction
	{
	public:
		inline CustomFunction(std::string const & name, std::vector<CustomFunctionParam> params)
			: name_(name), params_(params)
		{}

		inline std::string Name() const
		{
			return name_;
		}

		inline std::vector<CustomFunctionParam> const & Params() const
		{
			return params_;
		}

		inline FunctionHandle Handle() const
		{
			return handle_;
		}

		inline FunctionNameAndArity NameAndArity() const
		{
			return { name_, (unsigned)params_.size() };
		}

		inline void AssignHandle(FunctionHandle handle)
		{
			handle_ = handle;
		}

		bool ValidateArgs(OsiArgumentDesc const & params) const;
		void GenerateHeader(std::stringstream & ss) const;

	private:
		std::string name_;
		std::vector<CustomFunctionParam> params_;
		FunctionHandle handle_;

		bool ValidateParam(CustomFunctionParam const & param, OsiArgumentValue const & value) const;
	};

	class CustomCall : public CustomFunction
	{
	public:
		inline CustomCall(std::string const & name, std::vector<CustomFunctionParam> params,
			std::function<void (OsiArgumentDesc const &)> handler)
			: CustomFunction(name, std::move(params)), handler_(handler)
		{}

		bool Call(OsiArgumentDesc const & params);

	private:
		std::function<void(OsiArgumentDesc const &)> handler_;
	};

	class CustomQuery : public CustomFunction
	{
	public:
		inline CustomQuery(std::string const & name, std::vector<CustomFunctionParam> params,
			std::function<bool(OsiArgumentDesc &)> handler)
			: CustomFunction(name, std::move(params)), handler_(handler)
		{}

		bool Query(OsiArgumentDesc & params);

	private:
		std::function<bool(OsiArgumentDesc &)> handler_;
	};

	class CustomEvent : public CustomFunction
	{
	public:
		inline CustomEvent(std::string const & name, std::vector<CustomFunctionParam> params)
			: CustomFunction(name, std::move(params))
		{}
	};

	class CustomFunctionManager
	{
	public:
		// Arbitrary ID for custom extension functions
		static constexpr uint32_t CallClassId = 1337;
		static constexpr uint32_t QueryClassId = 1338;
		static constexpr uint32_t EventClassId = 1339;

		FunctionHandle Register(std::unique_ptr<CustomCall> call);
		FunctionHandle Register(std::unique_ptr<CustomQuery> qry);
		FunctionHandle Register(std::unique_ptr<CustomEvent> event);

		bool Call(FunctionHandle handle, OsiArgumentDesc const & params);
		bool Query(FunctionHandle handle, OsiArgumentDesc & params);

		std::string GenerateHeaders() const;

	private:
		std::unordered_map<FunctionNameAndArity, CustomFunction *> signatures_;
		std::vector<std::unique_ptr<CustomCall>> calls_;
		std::vector<std::unique_ptr<CustomQuery>> queries_;
		std::vector<std::unique_ptr<CustomEvent>> events_;

		void RegisterSignature(CustomFunction * func);
	};

	class CustomFunctionInjector
	{
	public:
		CustomFunctionInjector(class OsirisWrappers & wrappers, CustomFunctionManager & functions);

		void Initialize();

	private:
		OsirisWrappers & wrappers_;
		CustomFunctionManager & functions_;
		std::wstring storyHeaderPath_;
		HANDLE storyHeaderFile_{ NULL };
		bool extendingStory_{ false };

		void OnAfterGetFunctionMappings(void * Osiris, MappingInfo ** Mappings, uint32_t * MappingCount);
		bool CallWrapper(std::function<bool(uint32_t, OsiArgumentDesc *)> const & next, uint32_t handle, OsiArgumentDesc * params);
		bool QueryWrapper(std::function<bool(uint32_t, OsiArgumentDesc *)> const & next, uint32_t handle, OsiArgumentDesc * params);
		void ExtendStoryHeader(std::wstring const & headerPath);
		void OnCreateFile(LPCWSTR lpFileName,
			DWORD dwDesiredAccess,
			DWORD dwShareMode,
			LPSECURITY_ATTRIBUTES lpSecurityAttributes,
			DWORD dwCreationDisposition,
			DWORD dwFlagsAndAttributes,
			HANDLE hTemplateFile,
			HANDLE hFile);
		void OnCloseHandle(HANDLE hFile, BOOL bSucceeded);
	};
}
