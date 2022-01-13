local Generator = {}

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
    o.EmittedClasses = {}
    o.Text = ""
    self.__index = self
    return o
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

        table.insert(args, "self:" .. self:MakeTypeSignature(nil, cls))
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

function Generator:EmitComment(text)
    self.Text = self.Text .. "--- " .. text .. "\r\n"
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

function Generator:EmitClass(type)
    local name = self:MakeTypeName(type.TypeName)
    if type.ParentType ~= nil then
        name = name .. ":" .. self:MakeTypeName(type.ParentType.TypeName)
    end

    self:EmitComment("@class " .. name)

    local sortedMethods = {}
    for fname,ftype in pairs(type.Methods) do table.insert(sortedMethods, fname) end
    table.sort(sortedMethods)

    for i,fname in ipairs(sortedMethods) do
        self:EmitComment("@field " .. fname .. " " .. self:MakeTypeSignature(type, type.Methods[fname]))
    end

    local sortedMembers = {}
    for fname,ftype in pairs(type.Members) do table.insert(sortedMembers, fname) end
    table.sort(sortedMembers)

    for i,fname in ipairs(sortedMembers) do
        self:EmitComment("@field " .. fname .. " " .. self:MakeTypeSignature(type, type.Members[fname]))
    end
end

Ext.Types.GenerateIdeHelpers = function ()
    local gen = Generator:New()
    gen:Build()
    return gen.Text
end
