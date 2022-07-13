---@diagnostic disable

return {
["Ext_Utils.Include"] = {
Before = [[--- @overload fun(modGUID:string|nil, path:string, replaceGlobals:table|nil):any
--- @param path string The path to the script, relative to the Lua folder
--- @param modGUID string|nil The ModuleUUID value
--- @param replaceGlobals table|nil If set, the global environment of the script is replaced with this table
--- @return any returnedValue Whatever the script returned, if anything]],
}}