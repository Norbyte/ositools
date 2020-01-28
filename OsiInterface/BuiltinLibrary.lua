Ext._OnGameSessionLoading = function ()
    for i,callback in pairs(Ext.OnGameSessionLoading) do
        local status, err = xpcall(callback, debug.traceback)
        if not status then
            Ext.PrintError("Error during OnGameSessionLoading: ", err)
        end
    end
end

Ext.OnGameSessionLoading = {}

Ext._OnModuleLoading = function ()
    for i,callback in pairs(Ext.OnModuleLoading) do
        local status, err = xpcall(callback, debug.traceback)
        if not status then
            Ext.PrintError("Error during OnGameSessionLoading: ", err)
        end
    end
end

Ext.OnModuleLoading = {}

Ext._SkillGetDescriptionParam = function (...)
    for i,callback in pairs(Ext.SkillGetDescriptionParam) do
        local status, desc = xpcall(callback, debug.traceback, ...)
        if status then
			if desc ~= nil then
				return desc
			end
		else
            Ext.PrintError("Error during SkillGetDescriptionParam: ", desc)
        end
    end
end

Ext.SkillGetDescriptionParam = {}

Ext._StatusGetDescriptionParam = function (...)
    for i,callback in pairs(Ext.StatusGetDescriptionParam) do
        local status, desc = xpcall(callback, debug.traceback, ...)
        if status then
			if desc ~= nil then
				return desc
			end
		else
            Ext.PrintError("Error during StatusGetDescriptionParam: ", desc)
        end
    end
end

Ext.StatusGetDescriptionParam = {}

Ext._GetSkillDamage = function (...)
    for i,callback in pairs(Ext.GetSkillDamage) do
        local status, damageList, deathType = xpcall(callback, debug.traceback, ...)
        if status then
			if damageList ~= nil then
				return damageList, deathType
			end
		else
            Ext.PrintError("Error during GetSkillDamage: ", damageList)
        end
    end
end

Ext.GetSkillDamage = {}

Ext._CalculateTurnOrder = function (...)
    for i,callback in pairs(Ext.CalculateTurnOrder) do
        local status, turnOrder = xpcall(callback, debug.traceback, ...)
        if status then
			if turnOrder ~= nil then
				return turnOrder
			end
		else
            Ext.PrintError("Error during CalculateTurnOrder: ", turnOrder)
        end
    end
end

Ext.CalculateTurnOrder = {}

Ext.RegisterListener = function (type, fn)
	if type == "SessionLoading" then
		table.insert(Ext.OnGameSessionLoading, fn)
	elseif type == "ModuleLoading" then
		table.insert(Ext.OnModuleLoading, fn)
	elseif type == "SkillGetDescriptionParam" then
		table.insert(Ext.SkillGetDescriptionParam, fn)
	elseif type == "GetSkillDamage" then
		table.insert(Ext.GetSkillDamage, fn)
	elseif type == "StatusGetDescriptionParam" then
		table.insert(Ext.StatusGetDescriptionParam, fn)
	elseif type == "CalculateTurnOrder" then
		table.insert(Ext.CalculateTurnOrder, fn)
	else
		error("Unknown listener type: " .. type)
	end
end

dofile = function ()
	error("dofile() has been disabled for security reasons")
end

loadfile = function ()
	error("loadfile() has been disabled for security reasons")
end

load = function ()
	error("load() has been disabled for security reasons")
end

loadstring = function ()
	error("loadstring() has been disabled for security reasons")
end

rawget = function ()
	error("rawget() has been disabled for security reasons")
end

rawset = function ()
	error("rawset() has been disabled for security reasons")
end

rawequal = function ()
	error("rawequal() has been disabled for security reasons")
end

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
