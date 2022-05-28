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
    o.Text = ""
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
            if mod.name:sub(1, 8) == 'dse::esv' then
                self.NativeModules['ExtServer.' .. name] = mod
            elseif mod.name:sub(1, 8) == 'dse::ecl' then
                self.NativeModules['ExtClient.' .. name] = mod
            else
                self.NativeModules['Ext.' .. name] = mod
            end
        end
    else
        Ext.PrintWarning("Unable to load native class data; IDE helpers will not include annotations from C++ code")
        Ext.PrintWarning(res)
    end
end

function Generator:Build()
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
        elseif typeName == "vec2" or typeName == "vec3" or typeName == "vec4" or typeName == "ivec2" then
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
end

function Generator:MakeTypeName(type)
    type = string.gsub(type, "_%a", function (ns)
        return string.upper(string.sub(ns, 2))
    end)
    type = string.gsub(type, "%a+::", function (ns)
        return string.upper(string.sub(ns, 1, 1)) .. string.sub(ns, 2, -3)
    end)
    return type
end

function Generator:MakeTypeSignature(cls, type, forceExpand)
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
            table.insert(args, "self:" .. self:MakeTypeSignature(nil, cls))
        end
        
        for i,arg in ipairs(type.Params) do
            table.insert(args, "a" .. i .. ":" .. self:MakeTypeSignature(cls, arg))
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
    self.Text = self.Text .. "\r\n"
end

function Generator:EmitLine(text)
    self.Text = self.Text .. text .. "\r\n"
end

function Generator:EmitComment(text)
    self.Text = self.Text .. "--- " .. text .. "\r\n"
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
        decl = decl .. " | \"'" .. key .. "'\""
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

function Generator:EmitMethod(type, fname, nativeDefn)
    local nativeMethod = self:FindNativeMethod(fname, nativeDefn)

    if nativeMethod == nil then
        self:EmitComment("@field " .. fname .. " " .. self:MakeTypeSignature(type, type.Methods[fname]))
    else
        self:EmitFullMethodSignature(type, fname, type.Methods[fname], nativeMethod)
    end
end

function Generator:EmitModuleFunction(type, fname, nativeDefn)
    local nativeFunc = self:FindNativeFunction(fname, nativeDefn)

    if nativeFunc == nil then
        self:EmitComment("@field " .. fname .. " " .. self:MakeTypeSignature(nil, type.Methods[fname]))
    else
        self:EmitFullMethodSignature(type, fname, type.Methods[fname], nativeFunc)
    end
end

function Generator:EmitFullMethodSignature(cls, funcName, fun, nativeMethod)
    local argDescs = {}
    local args = {}

    local clsName = self:MakeTypeName(cls.TypeName)
    local helpersClsName = clsName:gsub("%.", "")

    if cls.Kind ~= "Module" then
        table.insert(argDescs, "--- @param self " .. helpersClsName)
        table.insert(args, "self")
    end
    
    for i,arg in ipairs(fun.Params) do
        table.insert(argDescs, "--- @param " .. nativeMethod.params[i].name .. " " .. self:MakeTypeSignature(cls, arg) .. " " ..  self.Trim(nativeMethod.params[i].description))
        table.insert(args, nativeMethod.params[i].name)
    end

    for i,arg in ipairs(fun.ReturnValues) do
        table.insert(argDescs, "--- @return " .. self:MakeTypeSignature(cls, arg))
    end

    local fun = "function " .. helpersClsName .. "." .. funcName .. "(" .. table.concat(args, ", ") .. ") end"
    local desc = table.concat(argDescs, "\r\n")

    local funcDesc = self.Trim(nativeMethod.description) .. "\r\n" .. "Location: " .. nativeMethod.implementation_file .. ":" .. nativeMethod.implementation_line

    self:EmitMultiLineComment(funcDesc)
    self.Text = self.Text .. desc .. "\r\n" .. fun .. "\r\n\r\n"
end

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
        if self:FindNativeMethod(fname, nativeDefn) ~= nil then
            table.insert(extendedMethodSigs, fname)
        else
            table.insert(basicMethodSigs, fname)
        end
    end
    
    for i,fname in ipairs(basicMethodSigs) do
        self:EmitMethod(type, fname, nativeDefn)
    end

    self:EmitLine(name .. ' = {}')
    self:EmitLine("")
    
    for i,fname in ipairs(extendedMethodSigs) do
        self:EmitMethod(type, fname, nativeDefn)
    end
end

function Generator:EmitModule(type)
    local helpersModuleName = type.TypeName:gsub("%.", "")
    self:EmitComment("@class " .. helpersModuleName)
    local nativeDefn = self.NativeModules[type.TypeName]

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

    self:EmitLine(helpersModuleName .. ' = {}')
    self:EmitLine("")
    
    for i,fname in ipairs(extendedFuncSigs) do
        self:EmitModuleFunction(type, fname, nativeDefn)
    end
end

Ext.Types.GenerateIdeHelpers = function ()
    local gen = Generator:New()
    gen:LoadNativeData()
    gen:Build()
    return gen.Text
end
