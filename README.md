# OsiTools

A toolkit for manipulating Divinity Original Sin 2 internals.

Features:
 - Lots of new Osiris calls and queries that expose previously unavailable functionality ([API Documentation]([https://github.com/Norbyte/ositools/blob/master/APIDocs.md](https://github.com/Norbyte/ositools/blob/master/APIDocs.md)))
 - Real-time debugging of Osiris code ([Usage](https://gist.github.com/Norbyte/8b7eb35cd17f799ea113636b15e1f967))
 - Logging of Osiris rule execution
 - Logging of Osiris compilation

### Configuration

The following configuration variables can be set in the `OsirisProxy.json` file:

| Variable | Type | Description |
|--|--|--|
| CreateConsole | Boolean | Creates a console window that logs extender internals. Mainly useful for debugging. |
| EnableLogging | Boolean | Enable logging of Osiris activity (rule evaluation, queries, etc.) to a log file. |
| LogCompile | Boolean | Log errors during Osiris story compilation to a log file. |
| LogDirectory | String | Directory where the generated Osiris logs will be stored. Default is `My Documents\OsirisLogs` |
| EnableExtensions | Boolean | Make the newly added Osiris calls and queries available ingame or in the editor. |
| EnableDebugger | Boolean | Enables the debugger interface |
| DebuggerPort | Integer | Port number the debugger will listen on (default 9999) |
