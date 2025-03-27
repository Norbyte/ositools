
local extMetatable = {
	__index = Ext,
    
    __newindex = function (t, k, v)
		if k == "GetHitChance" or k == "StatusGetEnterChance" then
			Ext.RegisterListener(k, v)
			Ext.PrintError("Setting Ext." .. k .. " is deprecated")
		else
			Ext.PrintError("Couldn't set Ext." .. k .. ": Please avoid extending the Ext table - it is dangerous and may break compatibility!")
		end
	end
}

-- Special bool to allow conditioning logic to avoid forcing hard-dependency with Pip's fork for mods
Ext.IsPipFork = true
Ext.PipForkVersion = 4

local oldExt = Ext
Ext = {}
setmetatable(Ext, extMetatable)


dofile = function ()
	error("dofile() has been disabled for security reasons")
end

loadfile = function ()
	error("loadfile() has been disabled for security reasons")
end

load = Ext.Utils.LoadString
loadstring = Ext.Utils.LoadString

local oldDebug = debug
debug = {
	traceback = oldDebug.traceback
}
oldDebug = nil

-- math.random replaced by custom implementation
math.random = Ext.Random
-- math.randomseed not implemented
math.randomseed = function ()
	error("math.randomseed() not implemented")
end

print = Ext.Utils.Print
