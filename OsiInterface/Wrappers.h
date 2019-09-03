#pragma once

#include <functional>

namespace osidbg {

	template <typename T>
	class WrappedFunction;

	template <typename R, typename... Params>
	class WrappedFunction<R(Params...)>
	{
	public:
		typedef R(*FuncType)(Params...);

		bool IsWrapped() const
		{
			return OriginalFunc != nullptr;
		}

		void Wrap(HMODULE Module, char * ProcName, FuncType NewFunction)
		{
			FARPROC ExportProc = GetProcAddress(Module, ProcName);
			if (ExportProc == NULL) {
				std::string errmsg("Could not locate export ");
				errmsg += ProcName;
				Fail(errmsg.c_str());
			}

			Wrap(ExportProc, NewFunction);
		}

		void Wrap(void * Function, FuncType NewFunction)
		{
			if (IsWrapped()) {
				throw std::runtime_error("Tried to wrap function multiple times");
			}

			OriginalFunc = Function;
			NewFunc = NewFunction;
			auto status = DetourAttachEx((PVOID *)&OriginalFunc, (PVOID)NewFunc, (PDETOUR_TRAMPOLINE *)&FuncTrampoline, NULL, NULL);
			if (status != NO_ERROR) {
				std::wstringstream ss;
				ss << "Detour attach failed: error " << status;
				Fail(ss.str().c_str());
			}
		}

		void Unwrap()
		{
			if (IsWrapped()) {
				DetourDetach((PVOID *)&OriginalFunc, (PVOID)NewFunc);
			}
		}

		inline R operator ()(Params... Args) const
		{
			return FuncTrampoline(std::forward<Params>(Args)...);
		}

	private:
		void * OriginalFunc{ nullptr };
		FuncType NewFunc{ nullptr };
		FuncType FuncTrampoline{ nullptr };
	};

	template <typename R, typename... Params>
	struct GetHookSignature
	{
		using Type = void(Params..., R);
	};

	template <typename... Params>
	struct GetHookSignature<void, Params...>
	{
		using Type = void(Params...);
	};

	template <class Tag, class T>
	class HookableFunction;

	template <class Tag, class R, class... Params>
	class HookableFunction<Tag, R(Params...)>
	{
	public:
		using FuncType = R(Params...);

		using PreHookType = void(Params...);
		using PreHookFuncType = std::function<PreHookType>;

		using PostHookType = typename GetHookSignature<R, Params...>::Type;
		using PostHookFuncType = std::function<PostHookType>;

		using WrapperHookType = R(std::function<FuncType> const &, Params...);
		using WrapperHookFuncType = std::function<WrapperHookType>;

		bool IsWrapped() const
		{
			return wrapped_.IsWrapped();
		}

		void Wrap(HMODULE Module, char * ProcName)
		{
			wrapped_.Wrap(Module, ProcName, &CallToTrampoline);

			if (gHook != nullptr) {
				Fail("Hook already registered");
			}

			gHook = this;
		}

		void Wrap(void * Function)
		{
			wrapped_.Wrap(Function, &CallToTrampoline);

			if (gHook != nullptr) {
				Fail("Hook already registered");
			}

			gHook = this;
		}

		void Unwrap()
		{
			wrapped_.Unwrap();
			gHook = nullptr;
		}

		void AddWrapper(WrapperHookFuncType wrapper)
		{
			auto nextHookIndex = (unsigned)wrapperHooks_.size() + 1;
			wrapperHooks_.push_back({
				wrapper,
				[=](Params... Args) {
					return this->CallWrapper(nextHookIndex, std::forward<Params>(Args)...);
				}
			});
		}

		void AddPreHook(PreHookFuncType hook)
		{
			preHooks_.push_back(hook);
		}

		void AddPostHook(PostHookFuncType hook)
		{
			postHooks_.push_back(hook);
		}

		inline R CallWithHooks(Params... Args) const
		{
			return CallWrapper(0, Args...);
		}

		inline R CallOriginal(Params... Args) const
		{
			return wrapped_(std::forward<Params>(Args)...);
		}

	private:
		struct WrapperHookInfo
		{
			WrapperHookFuncType hook;
			std::function<FuncType> forwarder;
		};

		WrappedFunction<R(Params...)> wrapped_;
		std::vector<WrapperHookInfo> wrapperHooks_;
		std::vector<PreHookFuncType> preHooks_;
		std::vector<PostHookFuncType> postHooks_;

		static HookableFunction<Tag, R(Params...)> * gHook;

		inline R CallWrapper(unsigned WrapperHookNum, Params... Args) const
		{
			if (WrapperHookNum < wrapperHooks_.size()) {
				auto const & hook = wrapperHooks_[WrapperHookNum];
				return hook.hook(std::ref(hook.forwarder), std::forward<Params>(Args)...);
			} else {
				for (auto const & hook : preHooks_) {
					hook(std::forward<Params>(Args)...);
				}

				if constexpr (std::is_same<void, R>::value) {
					wrapped_(std::forward<Params>(Args)...);

					for (auto const & hook : postHooks_) {
						hook(std::forward<Params>(Args)...);
					}
				} else {
					auto retval = wrapped_(std::forward<Params>(Args)...);

					for (auto const & hook : postHooks_) {
						hook(std::forward<Params>(Args)..., std::forward<decltype(retval)>(retval));
					}

					return retval;
				}
			}
		}

		static R CallToTrampoline(Params... Args)
		{
			return gHook->CallWithHooks(std::forward<Params>(Args)...);
		}
	};
}