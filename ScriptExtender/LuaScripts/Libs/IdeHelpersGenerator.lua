local _DEBUG = Ext.Debug.IsDeveloperMode()

local _format = string.format

local NEWLINE = "\r\n"

---@type {Specific:table<string,string>, Misc:string[]}
local _CustomEntries = Ext.Utils.Include(nil, "builtin://Libs/HelpersGenerator/CustomEntries.lua")
---@type table<string,{Before:string|nil, After:string|nil}>
local _CustomTypeEntries = Ext.Utils.Include(nil, "builtin://Libs/HelpersGenerator/CustomTypeEntries.lua")
---@type table<string,{Before:string|nil, After:string|nil}>
local _CustomFunctionExtras = Ext.Utils.Include(nil, "builtin://Libs/HelpersGenerator/CustomFunctionExtras.lua")
---@type string
local _OsiLines = Ext.Utils.Include(nil, "builtin://Libs/HelpersGenerator/Osi.lua")

local Generator = {}

function Generator.Trim(s)
    return s:gsub("^%s*(.-)%s*$", "%1")
end

function Generator.RTrim(s)
    return s:gsub("^(.-)%s*$", "%1")
end

Generator.ValueKindToLua = {
    Boolean = "boolean",
    Integer = "number",
    Float = "number",
    String = "string",
    Enumeration = "string",
    Any = "any"
}

local startingText = [[--- @diagnostic disable

--- Special global value that contains the current mod UUID during load
--- @type FixedString
ModuleUUID = "UUID"

--- Using a DB like a function will allow inserting new values into the database (ex. `Osi.DB_IsPlayer("02a77f1f-872b-49ca-91ab-32098c443beb")`  
--- @overload fun(...:string|number|nil)
--- @class OsiDatabase
local OsiDatabase = {}
--- Databases can be read using the Get method. The method checks its parameters against the database and only returns rows that match the query.  
--- The number of parameters passed to Get must be equivalent to the number of columns in the target database.  
--- Each parameter defines an (optional) filter on the corresponding column.  
--- If the parameter is nil, the column is not filtered (equivalent to passing _ in Osiris). If the parameter is not nil, only rows with matching values will be returned.
--- @vararg string|number|nil
--- @return table<integer,table<integer,string|number>>
function OsiDatabase:Get(...) end
--- The Delete method can be used to delete rows from databases.  
--- The number of parameters passed to Delete must be equivalent to the number of columns in the target database.  
--- Each parameter defines an (optional) filter on the corresponding column.  
--- If the parameter is nil, the column is not filtered (equivalent to passing _ in Osiris). If the parameter is not nil, only rows with matching values will be deleted. 
--- @vararg string|number|nil
function OsiDatabase:Delete(...) end

--- @class Osi
--- @field DB_IsPlayer OsiDatabase|fun(GUID:string) All player characters
--- @field DB_Origins OsiDatabase|fun(GUID:string) All origin characters
--- @field DB_Avatars OsiDatabase|fun(GUID:string) All player characters that were created in character creation, or that have an `AVATAR` tag
--- @field DB_CombatObjects OsiDatabase|fun(GUID:string, combatID:integer) All objects in combat
--- @field DB_CombatCharacters OsiDatabase|fun(GUID:string, combatID:integer) All characters in combat
--- @field DB_Dialogs OsiDatabase|fun(GUID:string, dialog:string)|fun(GUID1:string, GUID2:string, dialog:string)|fun(GUID1:string, GUID2:string, GUID3:string, dialog:string)|fun(GUID1:string, GUID2:string, GUID3:string, GUID4:string, dialog:string) All registered dialogs for objects, the most common being the version with a single character
Osi = {}

]]

function Generator:New()
    local o = {}
    setmetatable(o, self)
    o.Intrinsics = {}
    o.Builtins = {}
    o.Enumerations = {}
    o.Classes = {}
    o.Modules = {}
    o.NativeClasses = {}
    o.NativeModules = {}
    o.Text = startingText
    self.__index = self
    return o
end

function Generator:LoadNativeData()
    self.NativeClasses = {}
    self.NativeModules = {}

    local status, res = xpcall(function () 
        local libsJson = Ext.Utils.Include(nil, 'builtin://Libs/IdeHelpersNativeData.lua')
        local libs = Ext.Json.Parse(libsJson)

        return libs
    end, debug.traceback)

    if status == true then
        self.NativeClasses = res.classes
    
        for name,mod in pairs(res.modules) do
            self.NativeModules[name] = mod
        end
    else
        Ext.Utils.PrintWarning("Unable to load native class data; IDE helpers will not include annotations from C++ code")
        Ext.Utils.PrintWarning(res)
    end
end

function Generator:Build(addOsi)
    local types = Ext.Types.GetAllTypes()
    local sortedTypes = {}

    for i,typeName in ipairs(types) do 
        table.insert(sortedTypes, typeName)
    end

    table.sort(sortedTypes)

    for i,typeName in ipairs(sortedTypes) do
        local type = Ext.Types.GetTypeInfo(typeName)
        if type.Kind == "Object" then
            table.insert(self.Classes, type)
        elseif type.Kind == "Module" then
            table.insert(self.Modules, type)
        elseif type.Kind == "Enumeration" then
            table.insert(self.Enumerations, type)
        elseif type.Kind == "Boolean" or type.Kind == "Integer" or type.Kind == "Float" or type.Kind == "String" or type.Kind == "Any" then
            table.insert(self.Intrinsics, type)
        elseif typeName == "vec2" or typeName == "vec3" or typeName == "vec4" or typeName == "ivec2" or typeName == "ivec3" or typeName == "mat3" or typeName == "mat4" or typeName == "Version" then
            table.insert(self.Builtins, type)
        end
    end

    for i,type in ipairs(self.Intrinsics) do
        self:EmitIntrinsicType(type)
    end

    for i,type in ipairs(self.Builtins) do
        self:EmitBuiltinType(type)
    end

    self:EmitEmptyLine()
    self:EmitEmptyLine()

    for i,type in ipairs(self.Enumerations) do
        self:EmitEnumeration(type)
    end

    self:EmitEmptyLine()
    self:EmitEmptyLine()

    for i,type in ipairs(self.Classes) do
        self:EmitClass(type)
        self:EmitEmptyLine()
        self:EmitEmptyLine()
    end

    for i,type in ipairs(self.Modules) do
        self:EmitModule(type)
        self:EmitEmptyLine()
        self:EmitEmptyLine()
    end
    
    self:EmitExt("Client")
    self:EmitExt("Server")
    self:EmitExt(nil, true)
    if addOsi then
        self:EmitLine(_OsiLines)
    end
end

function Generator:MakeTypeName(type)
    -- Replace "_" with capitalization (mainly used for stats class names)
    type = string.gsub(type, "_[%a%d]", function (ns)
        return string.upper(string.sub(ns, 2))
    end)
    -- Replace namespace "::" with capitalization
    type = string.gsub(type, "[%a%d]+::", function (ns)
        return string.upper(string.sub(ns, 1, 1)) .. string.sub(ns, 2, -3)
    end)
    -- Replace template classes "T<V>" with underscore ("T_V")
    type = string.gsub(type, "<[%a%d]+>", function (n)
        return "_" .. string.sub(n, 2, -2)
    end)
    return type
end

function Generator:MakeTypeSignature(cls, type, forceExpand, nativeDefn)
    if type.IsBuiltin and forceExpand ~= true then
        return type.TypeName
    elseif type.Kind == "Any" then
        return "any"
    elseif type.Kind == "Nullable" then
        return self:MakeTypeSignature(cls, type.ParentType) .. "|nil"
    elseif type.Kind == "Array" then
        if type.ElementType.Kind == "Array" or type.ElementType.Kind == "Map" then
            return self:MakeTypeSignature(nil, type.ElementType) .. "[]"
        else
            return self:MakeTypeName(type.ElementType.TypeName) .. "[]"
        end
    elseif type.Kind == "Map" then
        return "table<" .. self:MakeTypeName(type.KeyType.TypeName) .. ", " .. self:MakeTypeSignature(nil, type.ElementType) .. ">"
    elseif type.Kind == "Function" then
        local args = {}
        local retval = {}

        if cls ~= nil then
            table.insert(args, "self: " .. self:MakeTypeSignature(nil, cls))
        end
        
        for i,arg in ipairs(type.Params) do
            if nativeDefn ~= nil then
                table.insert(args, nativeDefn.params[i].name .. ": " .. self:MakeTypeSignature(cls, arg))
            else
                table.insert(args, "a" .. i .. ": " .. self:MakeTypeSignature(cls, arg))
            end
        end

        for i,arg in ipairs(type.ReturnValues) do
            table.insert(retval, self:MakeTypeSignature(cls, arg))
        end

        local fun = "fun(" .. table.concat(args, ", ") .. ")"
        if #retval > 0 then
            fun = fun .. ":" .. table.concat(retval, ", ")
        end

        return fun
    else
        return self:MakeTypeName(type.TypeName)
    end
end

function Generator:EmitEmptyLine()
    self.Text = self.Text .. NEWLINE
end

function Generator:EmitLine(text, skipNewline)
    self.Text = self.Text .. text .. NEWLINE
    if not skipNewline then
        self.Text = self.Text .. NEWLINE
    end
end

function Generator:EmitComment(text)
    self.Text = self.Text .. "--- " .. text .. NEWLINE
end

function Generator:EmitMultiLineComment(text)
    for line in text:gmatch("([^\n]+)") do
       self:EmitComment(self.RTrim(line))
    end
end

function Generator:EmitAlias(name, definition)
    self:EmitComment("@alias " .. self:MakeTypeName(name) .. " " .. definition)
end

function Generator:EmitIntrinsicType(type)
    self:EmitAlias(type.TypeName, self.ValueKindToLua[type.Kind])
end

function Generator:EmitBuiltinType(type)
    self:EmitAlias(type.TypeName, self:MakeTypeSignature(nil, type, true))
end

function Generator:EmitEnumeration(type)
    local decl = "string"
    for key,value in pairs(type.EnumValues) do
        decl = _format("%s|\"%s\"", decl, key)
    end
    self:EmitAlias(type.TypeName, decl)
end

function Generator:FindNativeClass(type)
    if type.NativeName ~= nil then
        local name = string.gsub(type.NativeName, "struct ", '')
        name = string.gsub(name, "class ", '')
        return self.NativeClasses[name]
    end

    return nil
end

function Generator:FindNativeMethod(fname, nativeCls)
    local nativeMethod = nil
    if nativeCls ~= nil then
        nativeMethod = nativeCls.methods[fname]
        if nativeMethod == nil then
            nativeMethod = nativeCls.methods["Lua" .. fname]
        end
    end

    return nativeMethod
end

function Generator:FindNativeFunction(fname, nativeNs)
    local nativeMethod = nil
    if nativeNs ~= nil then
        nativeMethod = nativeNs.functions[fname]
    end

    return nativeMethod
end

function Generator:MethodNeedsFullSignature(nativeMethod)
    if nativeMethod == nil then
        return false
    end

    if #self.Trim(nativeMethod.description) > 0 then
        return true
    end

    for i,fun in pairs(nativeMethod.params) do
        if #self.Trim(nativeMethod.description) > 0 then
            return true
        end
    end

    return false
end

function Generator:EmitMethod(type, fname, nativeDefn)
    local nativeMethod = self:FindNativeMethod(fname, nativeDefn)

    if self:MethodNeedsFullSignature(nativeMethod) then
        self:EmitFullMethodSignature(type, fname, type.Methods[fname], nativeMethod)
    else
        self:EmitComment("@field " .. fname .. " " .. self:MakeTypeSignature(type, type.Methods[fname],  false, nativeMethod))
    end
end

function Generator:EmitModuleFunction(type, fname, nativeDefn, afterText)
    local nativeFunc = self:FindNativeFunction(fname, nativeDefn)

    if nativeFunc == nil then
        self:EmitComment("@field " .. fname .. " " .. self:MakeTypeSignature(nil, type.Methods[fname]))
    else
        self:EmitFullMethodSignature(type, fname, type.Methods[fname], nativeFunc, afterText)
    end
end

function Generator:EmitFullMethodSignature(cls, funcName, fun, nativeMethod, afterText)
    local argDescs = {}
    local args = {}

    local clsName
    if cls.Kind == "Module" then
        clsName = self:MakeModuleTypeName(cls)
    else
        clsName = self:MakeTypeName(cls.TypeName)
    end

    local helpersClsName = clsName:gsub("%.", "")

    for i,arg in ipairs(fun.Params) do
        table.insert(argDescs, "--- @param " .. nativeMethod.params[i].name .. " " .. self:MakeTypeSignature(cls, arg) .. " " ..  self.Trim(nativeMethod.params[i].description))
        table.insert(args, nativeMethod.params[i].name)
    end

    for i,arg in ipairs(fun.ReturnValues) do
        table.insert(argDescs, "--- @return " .. self:MakeTypeSignature(cls, arg))
    end

    local fun = "function " .. helpersClsName
    if cls.Kind ~= "Module" then
        fun = fun .. ":"
    else
        fun = fun .. "."
    end

    fun = fun .. funcName .. "(" .. table.concat(args, ", ") .. ") end"
    local desc = table.concat(argDescs, NEWLINE)

    local funcDesc = self.Trim(nativeMethod.description)
    if nativeMethod.implementation_file ~= nil and #funcDesc > 0 then
        funcDesc = funcDesc .. NEWLINE .. "Location: " .. nativeMethod.implementation_file .. ":" .. nativeMethod.implementation_line
    end

    if #funcDesc > 0 then
        self:EmitMultiLineComment(funcDesc)
    end

    if desc ~= "" then
        self.Text = self.Text .. desc .. NEWLINE .. fun
    else
        self.Text = self.Text .. fun
    end
    if afterText then
        self.Text = self.Text .. NEWLINE .. afterText
    end
    self.Text = self.Text .. NEWLINE .. NEWLINE
end

local serverEventParamsPattern = "EsvLua(%a+)EventParams"
local clientEventParamsPattern = "EclLua(%a+)EventParams"
local bothContextEventParamsPattern = "(%a+)EventParams"

local eventTypeGenerationData = {}
local eventTypeGenerationDataIndex = {}
local EVENT_NAME_SWAP = {
    GameStateChange = "GameStateChanged",
    LuaTick = "Tick",
    LuaConsole = "DoConsoleCommand",
}
local IGNORE_PARAMS = {
    LuaEmptyEventParams = true
}

function Generator:EmitClass(type)
    local name = self:MakeTypeName(type.TypeName)
    local nameWithParent = name
    if type.ParentType ~= nil then
        nameWithParent = nameWithParent .. " : " .. self:MakeTypeName(type.ParentType.TypeName)
    end

    local nativeDefn = self:FindNativeClass(type)

    self:EmitComment("@class " .. nameWithParent)

    local sortedMembers = {}
    for fname,ftype in pairs(type.Members) do table.insert(sortedMembers, fname) end
    table.sort(sortedMembers)

    for i,fname in ipairs(sortedMembers) do
        self:EmitComment("@field " .. fname .. " " .. self:MakeTypeSignature(type, type.Members[fname]))
    end

    local sortedMethods = {}
    for fname,ftype in pairs(type.Methods) do table.insert(sortedMethods, fname) end
    table.sort(sortedMethods)

    local basicMethodSigs = {}
    local extendedMethodSigs = {}

    for i,fname in ipairs(sortedMethods) do
        local nativeMethod = self:FindNativeMethod(fname, nativeDefn)
        if self:MethodNeedsFullSignature(nativeMethod) then
            table.insert(extendedMethodSigs, fname)
        else
            table.insert(basicMethodSigs, fname)
        end
    end
    
    for i,fname in ipairs(basicMethodSigs) do
        self:EmitMethod(type, fname, nativeDefn)
    end

    if #extendedMethodSigs > 0 then
        self:EmitLine('local ' .. name .. ' = {}')
        self:EmitLine("")
        
        for i,fname in ipairs(extendedMethodSigs) do
            self:EmitMethod(type, fname, nativeDefn)
        end
    end

    if not IGNORE_PARAMS[name] and string.find(name, "EventParams") then
        local context = "any"
        local _,_,eventName = string.find(name, serverEventParamsPattern)
        if not eventName then
            _,_,eventName = string.find(name, clientEventParamsPattern)
            if eventName then
                context = "client"
            else
                _,_,eventName = string.find(name, bothContextEventParamsPattern)
            end
        else
            context = "server"
        end
        if eventName then
            if EVENT_NAME_SWAP[eventName] then
                eventName = EVENT_NAME_SWAP[eventName]
            else
                eventName = eventName:gsub("^Lua", "")
            end
            local lastIndex = eventTypeGenerationDataIndex[eventName]
            if lastIndex == nil then
                lastIndex = #eventTypeGenerationData+1
            else
                local lastData = eventTypeGenerationData[lastIndex]
                name = lastData.Type .. "|" .. name
                context = "any"
            end
            eventTypeGenerationData[lastIndex] = {Type = name, Event = eventName, Context = context}
            eventTypeGenerationDataIndex[eventName] = lastIndex
        end
    end
end


function Generator:MakeModuleTypeName(type)
    local name = type.NativeName:gsub("%.", "")
    if type.ModuleRole ~= "Both" then
        name = type.ModuleRole .. name
    end

    return "Ext_" .. name
end

function Generator:EmitModule(type)
    local helpersModuleName = self:MakeModuleTypeName(type)
    local nativeModuleName = type.NativeName
    if type.ModuleRole ~= "Both" then
        nativeModuleName = type.ModuleRole .. nativeModuleName
    end

    self:EmitComment("@class " .. helpersModuleName)
    local nativeDefn = self.NativeModules[nativeModuleName]

    local sortedFuncs = {}
    for fname,ftype in pairs(type.Methods) do table.insert(sortedFuncs, fname) end
    table.sort(sortedFuncs)

    local basicFuncSigs = {}
    local extendedFuncSigs = {}

    for i,fname in ipairs(sortedFuncs) do
        if self:FindNativeFunction(fname, nativeDefn) ~= nil then
            table.insert(extendedFuncSigs, fname)
        else
            table.insert(basicFuncSigs, fname)
        end
    end
    
    for i,fname in ipairs(basicFuncSigs) do
        self:EmitModuleFunction(type, fname, nativeDefn)
    end
    local customText = _CustomTypeEntries[helpersModuleName]
    if customText and customText.Before then
        self:EmitLine(customText.Before)
    end

    self:EmitLine('local ' .. helpersModuleName .. ' = {}')
    if customText and customText.After then
        self:EmitLine(customText.After)
    end
    self:EmitEmptyLine()
    
    for i,fname in ipairs(extendedFuncSigs) do
        local afterText = nil
        if nativeDefn ~= nil then
            local functionAdditions = _CustomFunctionExtras[helpersModuleName.."."..fname]
            if functionAdditions then
                if functionAdditions.Before then
                    self:EmitLine(functionAdditions.Before, true)
                end
                afterText = functionAdditions.After
            end
        end
        self:EmitModuleFunction(type, fname, nativeDefn, afterText)
    end
end

local function GenerateSubscriptionEvents(self)
    for _,k in pairs(Ext._Internal._PublishedSharedEvents) do
        if not eventTypeGenerationDataIndex[k] then
            if _DEBUG then
                Ext.Utils.PrintWarning("Found unregistered event", k)
            end
            eventTypeGenerationData[#eventTypeGenerationData+1] = {Type="LuaEmptyEventParams", Event = k, Context = "any"}
        end
    end
    table.sort(eventTypeGenerationData, function(a,b) return a.Event < b.Event end)
    for _,v in ipairs(eventTypeGenerationData) do
        if v.Context == "server" then
            self:EmitComment("🔨**Server-Only**🔨  ")
        elseif v.Context == "client" then
            self:EmitComment("🔧**Client-Only**🔧  ")
        else
            self:EmitComment("🔨🔧**Server/Client**🔧🔨  ")
        end
        self:EmitComment(_format("@type SubscribableEvent<%s>  ", v.Type))
        self:EmitLine(_format('Ext.Events.%s = {}', v.Event))
    end
end

function Generator:EmitExt(role, declareGlobal)
    self:EmitComment("@class Ext" .. (role or ""))

    local aliases = {}
    for i,mod in ipairs(self.Modules) do
        if role == nil or mod.ModuleRole == "Both" or mod.ModuleRole == role then
            local helpersModuleName = self:MakeModuleTypeName(mod)
            if aliases[mod.NativeName] ~= nil then
                aliases[mod.NativeName] = aliases[mod.NativeName] .. "|" .. helpersModuleName
            else
                aliases[mod.NativeName] = helpersModuleName
            end
        end
    end

    local emitted = {}
    for i,mod in ipairs(self.Modules) do
        if role == nil or mod.ModuleRole == "Both" or mod.ModuleRole == role then
            local helpersModuleName = self:MakeModuleTypeName(mod)
            if emitted[mod.NativeName] == nil then
                self:EmitComment("@field " .. mod.NativeName .. " " .. (aliases[mod.NativeName] or helpersModuleName))
                emitted[mod.NativeName] = true
            end
            if mod.ModuleRole ~= "Both" then
                self:EmitComment("@field " .. mod.ModuleRole .. mod.NativeName .. " " .. helpersModuleName)
            end
        end
    end

    if declareGlobal then
        self:EmitLine("Ext = {Events = {}}")
        self:EmitEmptyLine()
        self:EmitLine("--#region Extender Events")
        self:EmitEmptyLine()
        for k,v in pairs(_CustomEntries.Specific) do
            self:EmitLine(v)
            if k == "SubscribableEventType" then
                GenerateSubscriptionEvents(self)
            end
            self:EmitEmptyLine()
        end
        self:EmitLine("--#endregion")
        self:EmitEmptyLine()
        for _,v in ipairs(_CustomEntries.Misc) do
            self:EmitLine(v)
            self:EmitEmptyLine()
        end
    end
    self:EmitEmptyLine()
    self:EmitEmptyLine()
end

---@param outputPath string|nil
---@param addOsi boolean|nil
Ext.Types.GenerateIdeHelpers = function (outputPath, addOsi)
    eventTypeGenerationData = {}
    eventTypeGenerationDataIndex = {}
    local gen = Generator:New()
    gen:LoadNativeData()
    gen:Build(addOsi)
    if outputPath then
        Ext.IO.SaveFile(outputPath, gen.Text)
    end
    return gen.Text
end