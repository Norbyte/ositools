local _G = _G

Ext._Internal = {}
local _I = Ext._Internal

Ext._LoadedFiles = {}
-- Table to hold debugger expression evaluation results
Ext._EVAL_ROOTS_ = {}
Mods = {}

_I._PublishedSharedEvents = {
	"ModuleLoadStarted",
	"ModuleLoading",
	"StatsLoaded",
	"ModuleResume",
	"SessionLoading",
	"SessionLoaded",
	"GameStateChanged",
	"ResetCompleted"
}

Ext._WarnDeprecated56 = function (msg)
	Ext.PrintError(msg)
	Ext.PrintError("See https://github.com/Norbyte/ositools/blob/master/LuaAPIDocs.md#migrating-from-v55-to-v56 for more info.")
end

Ext._EngineCallback1 = function (event, ...)
    for i,callback in pairs(Ext._Listeners[event]) do
        local status, result = xpcall(callback, debug.traceback, ...)
        if status then
			if result ~= nil then
				return result
			end
		else
            Ext.PrintError("Error during " .. event .. ": ", result)
        end
    end
end

Ext._EngineCallback2 = function (event, ...)
    for i,callback in pairs(Ext._Listeners[event]) do
        local status, result1, result2 = xpcall(callback, debug.traceback, ...)
        if status then
			if result1 ~= nil then
				return result1, result2
			end
		else
            Ext.PrintError("Error during " .. event .. ": ", result1)
        end
    end
end

Ext._GetHitChance = function (...)
    return Ext._EngineCallback1("GetHitChance", ...)
end

Ext._GetSkillAPCost = function (...)
    return Ext._EngineCallback2("GetSkillAPCost", ...)
end

Ext.Require = function (mod, path)
	if ModuleUUID == nil then
		Ext.PrintWarning("Calling Ext.Require() after the module was loaded is deprecated!");
	end

	local fullName
	if path == nil then
		fullName = ModuleUUID .. "/" .. mod
	else
		fullName = mod .. "/" .. path
	end

	if Ext._LoadedFiles[fullName] ~= nil then
		return Ext._LoadedFiles[fullName]
	end
	
	local env
	-- LuaJIT workaround
	if getfenv ~= nil then
		env = getfenv(2)
	end

	local loaded
	if path == nil then
		loaded = {Ext.Include(ModuleUUID, mod, env)}
	else
		loaded = {Ext.Include(mod, path, env)}
	end

	Ext._LoadedFiles[fullName] = loaded
	return table.unpack(loaded)
end

Ext._LoadBootstrap = function (path, modTable)
	local env = {
		-- Put frequently used items directly into the table for faster access
		type = type,
		tostring = tostring,
		tonumber = tonumber,
		pairs = pairs,
		ipairs = ipairs,
		print = print,
		error = error,
		next = next,

		string = string,
		math = math,
		table = table,

		Ext = Ext,
		Osi = Osi,
		Game = Game,
		Sandboxed = true,

		ModuleUUID = ModuleUUID
	}
	-- The rest are accessed via __index
	setmetatable(env, {__index = _G})
	Mods[modTable] = env
	
	env._G = env
	Ext.Include(ModuleUUID, path, env)
end

-- Used by the Lua debug adapter to store intermediate evaluation results.
-- Should not be used manually!
Ext.DebugEvaluate = function (retval)
	if type(retval) ~= "table" then
		return retval
	else
		local idx = #Ext._EVAL_ROOTS_ + 1
		Ext._EVAL_ROOTS_[idx] = retval
		return retval, idx
	end
end

-- Custom skill property registration
Ext._SkillPropertyTypes = {}

Ext.RegisterSkillProperty = function (name, proto)
	Ext._SkillPropertyTypes[name] = proto
end

-- Helper for dumping variables in console
Ext.Dump = function (val)
	Ext.Print(Ext.Json.Stringify(val, true, true, true))
end

-- Global helper aliases for Ext.Dump, Ext.Print
_D = Ext.Dump
_P = Ext.Print

-- Backwards compatibility with old JSON APIs
Ext.JsonStringify = Ext.Json.Stringify
Ext.JsonParse = Ext.Json.Parse
