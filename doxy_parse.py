from sys import implementation
from xml.etree import ElementTree
from os import listdir
from os.path import isfile, join
import json

class DescriptionMetadata:
    text = ''
    
    def __init__(self):
        self.attributes = {}

    def parse(self, n: ElementTree.Element):
        if n is None:
            return

        for para in n.findall('para'):
            if para.text is not None:
                if para.text.startswith('<'):
                    try:
                        ele = ElementTree.fromstring('<root>' + para.text + '</root>')
                        for tag in list(ele):
                            desc = {'text': tag.text}
                            for k,v in tag.attrib.items():
                                desc[k] = v
                            self.attributes[tag.tag] = desc
                    except ElementTree.ParseError:
                        print('Warning: Failed to parse annotation: ' + para.text)
        self.text += self.parse_text(n)

    def tag_wrap(self, text: str, tag: str):
        if tag == 'computeroutput':
            return '`' + text + '`'
        if tag == 'sp':
            return ' ' + text
        if tag == 'listitem':
            return " - " + text
        if tag == 'programlisting':
            return "\r\n```lua\r\n" + text + "```"
        return text

    def parse_text(self, n: ElementTree.Element):
        if n.tag == 'para' and n.text is not None and n.text.startswith('<'):
            return ''

        text = ''
        if n.text is not None and n.tag != 'briefdescription' and n.tag != 'detaileddescription' and n.tag != 'parameterdescription':
            text += n.text
        for ele in list(n):
            if ele.tag != 'parameterlist' and ele.tag != 'simplesect':
                text += self.parse_text(ele)
                if ele.tail is not None:
                    text += ele.tail
        return self.tag_wrap(text, n.tag)

def parse_description_metadata(n: ElementTree.Element) -> DescriptionMetadata:
    meta = DescriptionMetadata()
    meta.parse(n.find('briefdescription'))
    meta.parse(n.find('detaileddescription'))
    meta.parse(n.find('parameterdescription'))
    return meta

class FunctionParamDefn:
    type = None
    name = ''
    description = ''
    is_internal = False

    def from_xml(self, n: ElementTree.Element):
        self.type = n.find('type').text
        self.name = n.find('declname').text
        self.description = ''
        self.is_internal = (self.name == 'L')

    def to_json(self):
        return {
            "name": self.name,
            "type": self.type,
            "description": self.description
        }
        

class FunctionDefn:
    inspected = False
    name = None
    namespace = None
    exported = False
    export_name = None
    qualified_name = None
    description = None
    params = None
    definition = None
    implementation_file = None
    implementation_line = None

    def from_xml(self, n: ElementTree.Element, ns):
        self.namespace = ns
        desc = parse_description_metadata(n)
        if 'lua_export' in desc.attributes:
            self.exported = True

        self.description = desc.text
        self.parse_definition(n)
        if self.exported:
            self.export_name = desc.attributes['lua_export']['text'] or self.name
        else:
            self.export_name = self.name

    def dump(self):
        print('Exported function: ' + self.namespace.name + '::' + self.name + ' -> ' + (self.namespace.export_name or self.namespace.name) + '.' + self.export_name)
        print('Description: ' + self.description.strip())
        print('')
        for param in list(self.params):
            if not param.is_internal:
                print(' - Param ' + param.name + ': ' + (param.description or '').strip())
    
    def find_param_list(self, n: ElementTree.Element):
        for para in n.find('detaileddescription').findall('para'):
            params = para.find('parameterlist')
            if params is not None:
                return params

        return None

    def parse_definition(self, n: ElementTree.Element):
        self.inspected = True
        # FIXME - parse ref?
        # <type><ref refid="structdse_1_1esv_1_1_surface_action" kindref="compound">SurfaceAction</ref> *</type>
        self.type = n.find('type').text
        self.name = n.find('name').text
        self.definition = n.find('definition').text + n.find('argsstring').text
        self.qualified_name = n.find('qualifiedname').text
        loc = n.find('location')
        if 'bodyfile' in loc.attrib:
            self.implementation_file = loc.attrib['bodyfile']
            self.implementation_line = int(loc.attrib['bodystart'])
        self.params = []

        for param in n.findall('param'):
            name = param.find('declname')
            # Lua context is an internal parameter only
            if name is not None:
                paramDesc = FunctionParamDefn()
                paramDesc.from_xml(param)
                self.params.append(paramDesc)

        paramsByName = {p.name: p for p in self.params}

        params = self.find_param_list(n)
        if params is not None:
            for param in params.findall('parameteritem'):
                name = param.find('parameternamelist').find('parametername').text
                paramDesc = paramsByName.get(name)
                if paramDesc is None:
                    paramDesc = FunctionParamDefn()
                    paramDesc.name = name
                    self.params.append(paramDesc)

                description = parse_description_metadata(param)
                paramDesc.description = description.text

    def to_json(self):
        defn = {
            "name": self.export_name or self.name,
            "qualified_name": self.qualified_name,
            "description": self.description,
            "definition": self.definition,
            "implementation_file": self.implementation_file,
            "implementation_line": self.implementation_line,
            "params": []
        }

        for param in self.params:
            if not param.is_internal:
                defn["params"].append(param.to_json())

        return defn

    def has_any_annotations(self) -> bool:
        return self.exported or (self.description is not None and len(self.description) > 0) or (self.params is not None and len(self.params) > 0)
        

class PropertyDefn:
    name = None
    type = None
    description = None

    def from_xml(self, n: ElementTree.Element):
        self.type = n.find('type').text
        self.name = n.find('name').text
        desc = parse_description_metadata(n)
        self.description = desc.text

    def to_json(self):
        return {
            "name": self.name,
            "type": self.type,
            "description": self.description
        }

    def has_any_annotations(self) -> bool:
        return self.description is not None and len(self.description) > 0


class NamespaceDefn:
    name = ''
    exported = False
    export_name = None

    def __init__(self):
        self.functions = {}
        self.classes = {}
        self.class_names = []

    def from_xml(self, n: ElementTree.Element):
        self.name = n.find('compoundname').text
        desc = parse_description_metadata(n)
        if 'lua_module' in desc.attributes:
            self.exported = True
            self.export_name = desc.attributes['lua_module']['text']

        self.parse_definition(n)

    def parse_definition(self, n: ElementTree.Element):
        for section in list(n):
            if section.tag == 'sectiondef' and section.attrib['kind'] == 'func':
                self.parse_function_section(section)
            if section.tag == 'innerclass':
                self.class_names.append(section.attrib['refid'])

    def parse_function_section(self, n: ElementTree.Element):
        for memberdef in list(n):
            if memberdef.tag == 'memberdef' and memberdef.attrib['kind'] == 'function':
                fun = FunctionDefn()
                fun.from_xml(memberdef, self)
                if fun.exported:
                    self.functions[fun.name] = fun

    def to_json(self):
        defns = {
            "name": self.name,
            "functions": {},
            "classes": {}
        }

        for fun in self.functions.values():
            if fun.exported:
                defns["functions"][fun.export_name] = fun.to_json()
        
        for cls in self.classes.values():
            if cls.has_any_annotations():
                defns["classes"][cls.name] = cls.to_json()

        return defns


class ClassDefn:
    name = ''
    export_name = ''
    exported = False

    def __init__(self):
        self.properties = {}
        self.methods = {}

    def from_xml(self, n: ElementTree.Element):
        self.name = n.find('compoundname').text
        desc = parse_description_metadata(n)
        # FIXME - filter for only exported classes!
        self.exported = True
        if 'lua_export' in desc.attributes:
            self.exported = True

        if self.exported:
            self.parse_definition(n)

    def dump(self):
        print('Exported class: ' + self.name)
        for n,method in self.methods.items():
            print(method)
        for n,prop in self.properties.items():
            print(prop.name, prop.type, prop.description)


    def parse_definition(self, n: ElementTree.Element):
        for section in list(n):
            if section.tag == 'sectiondef' and section.attrib['kind'] == 'public-attrib':
                self.parse_property_section(section)
            if section.tag == 'sectiondef' and section.attrib['kind'] == 'public-func':
                self.parse_function_section(section)

    def parse_function_section(self, n: ElementTree.Element):
        for memberdef in list(n):
            if memberdef.tag == 'memberdef' and memberdef.attrib['kind'] == 'function':
                fun = FunctionDefn()
                fun.from_xml(memberdef, self)
                if fun.has_any_annotations():
                    self.methods[fun.name] = fun

    def parse_property_section(self, n: ElementTree.Element):
        for memberdef in list(n):
            if memberdef.tag == 'memberdef' and memberdef.attrib['kind'] == 'variable':
                prop = PropertyDefn()
                prop.from_xml(memberdef)
                self.properties[prop.name] = prop

    def has_any_annotations(self):
        for prop in self.properties.values():
            if prop.has_any_annotations():
                return True

        for fun in self.methods.values():
            if fun.has_any_annotations():
                return True

        return False

    def to_json(self):
        defns = {
            "name": self.export_name or self.name,
            "methods": {},
            "properties": {}
        }

        for fun in self.methods.values():
            defns["methods"][fun.export_name] = fun.to_json()
        
        for prop in self.properties.values():
            if prop.description is not None and len(prop.description) > 0:
                defns["properties"][prop.name] = prop.to_json()

        return defns


class DoxyMetadata:
    def __init__(self):
        self.namespaces = {}
        self.classes = {}

    def parse_namespace(self, n: ElementTree.Element):
        name = n.find('compoundname').text
        ns = self.namespaces.get(name) or NamespaceDefn()
        ns.from_xml(n)
        if ns.exported:
            self.namespaces[name] = ns

    def parse_struct(self, n: ElementTree.Element):
        name = n.find('compoundname').text
        ns = self.classes.get(name) or ClassDefn()
        ns.from_xml(n)
        if ns.exported:
            self.classes[name] = ns

    def parse_doxygen_xml(self, path):
        tree = ElementTree.parse(path)
        root = tree.getroot()
        for doxydef in root:
            if doxydef.tag == 'compounddef':
                if doxydef.attrib['kind'] == 'namespace':
                    self.parse_namespace(doxydef)
                if doxydef.attrib['kind'] == 'struct' or doxydef.attrib['kind'] == 'class':
                    self.parse_struct(doxydef)

    def parse_doxygen_dir(self, path):
        for f in listdir(path):
            f = join(path, f)
            if isfile(f):
                self.parse_doxygen_xml(f)

    def to_json(self):
        defns = {
            "modules": {},
            "classes": {}
        }

        for ns in self.namespaces.values():
            if ns.exported:
                defns["modules"][ns.export_name] = ns.to_json()
        
        for cls in self.classes.values():
            if cls.has_any_annotations():
                defns["classes"][cls.name] = cls.to_json()

        return defns

    def lua_export(self, path: str):
        lua = json.dumps(self.to_json())
        lua = "return [[" + lua + "]]"
        with open(path, 'w') as f:
            f.write(lua)


meta = DoxyMetadata()
meta.parse_doxygen_dir('ScriptExtenderTypeGen/Doxygen/xml')
meta.lua_export('ScriptExtender/LuaScripts/Libs/IdeHelpersNativeData.lua')
