# Norbyte's Divinity Script Extender

Downloads available [>>>HERE<<<](https://github.com/Norbyte/ositools/releases/latest).

The Script Extender toolkit adds previously unavailable features to the scripting language of the game.

Features:
 - Lots of new Osiris calls and queries ([API Documentation](https://github.com/Norbyte/ositools/blob/master/Docs/APIDocs.md))
 - Lua scripting ([API Documentation](https://github.com/Norbyte/ositools/blob/master/Docs/LuaAPIDocs.md))
 - Real-time debugging of Osiris code ([Usage](https://gist.github.com/Norbyte/8b7eb35cd17f799ea113636b15e1f967))
 - Significant improvements in modded game loading times
 - Debug logging of Osiris rule execution/compilation

### Configuration

The following configuration variables can be set in the `OsirisExtenderSettings.json` file:

| Variable | Type | Description |
|--|--|--|
| CreateConsole | Boolean | Creates a console window that logs extender internals. Mainly useful for debugging. Defaults to `false`. |
| EnableLogging | Boolean | Enables logging of Osiris activity (rule evaluation, queries, etc.) to a log file. Defaults to `false`. |
| LogRuntime | Boolean | Logs extender console and script output to a log file. Defaults to `false`. |
| LogCompile | Boolean | Logs Osiris story compilation to a log file. Defaults to `false`. |
| LogFailedCompile | Boolean | Logs errors during Osiris story compilation to a log file. Defaults to `true`. |
| LogDirectory | String | Directory where the generated Osiris logs will be stored. Defaults to `My Documents\OsirisLogs` |
| EnableExtensions | Boolean | Makes the Osiris extension functionality available ingame or in the editor. Defaults to `true`. |
| SendCrashReports | Boolean | Uploads minidumps to the crash report collection server after a game crash. Defaults to `true`. |
| ForceCrashReporting | Boolean | Allows crash dumps to be sent to the crash report collection server even if they are not detected to be related to the extender. Defaults to `false`. |
| DeveloperMode | Boolean | Enables various debug functionality for development purposes, such as logging messages from Iggy/Flash, and makes `Ext.Debug.IsDeveloperMode()` return `true`. Defaults to `false`. |
| DisableModValidation | Boolean | Disables module hashing when loading modules. Defaults to `true`. |
| EnableAchievements | Boolean | Re-enables achievements for modded games. Defaults to `true`. |
| EnableDebugger | Boolean | Enables the Osiris debugger interface. Defaults to `false`. |
| DebuggerPort | Integer | Port number the Osiris debugger will listen on. Defaults to `9999`. |
| EnableLuaDebugger | Boolean | Enables the Lua debugger interface. Defaults to `false`. |
| LuaDebuggerPort | Integer | Port number the Lua debugger will listen on. Defaults to `9998` |
| DefaultToClientConsole | Boolean | Makes the console default to the client context instead of server. Defaults to `false`. |
| ClearOnReset | Boolean | Clears the console window upon a manual Lua reset from the console. Defaults to `false`. |
| OptimizeHashing | Boolean | Circumvents an issue in the game's mod hashing logic that results in an exponential increase in loading times when using many mods. Defaults to `true`. |
| ShowPerfWarnings | Boolean | Logs warnings when the server thread is overloaded. Defaults to `false`. |
| SyncNetworkStrings | Boolean | Fixes a desync issue if there is a mismatch of content in mods between the client and server. Defaults to `true`. |
| LuaBuiltinResourceDirectory | String | Overwrites the directory that built-in Lua scripts are loaded from. Can be used to test changes to these scripts without needing to rebuild the extender. The built-in scripts are in `ScriptExtender\LuaScripts`. | 