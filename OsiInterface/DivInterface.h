#include <cstdint>
#include <array>
#include <vector>
#include <set>
#include <map>

namespace eoc
{

#pragma pack(push, 1)
enum class ValueType : uint8_t
{
	None = 0,
	Integer = 1,
	Integer64 = 2,
	Real = 3,
	String = 4,
	GuidString = 5,
	CharacterGuid = 6,
	ItemGuid = 7,
	TriggerGuid = 8,
	SplineGuid = 9,
	LevelTemplateGuid = 10,
	Undefined = 0x7f
};

struct CallParam
{
	ValueType Type;
	uint8_t __Padding[7];
	union {
		char * String;
		int32_t Int;
		int64_t Int64;
		float Real;
	} Value;
	CallParam * NextParam;
};

enum class EoCFunctionArgumentType : uint32_t
{
	InParam = 1,
	OutParam = 2
};

struct EoCFunctionArgument
{
	char const * Name;
	ValueType Type;
	uint8_t __Padding[3];
	EoCFunctionArgumentType ArgType;
};

struct EoCCallParam
{
	EoCFunctionArgument * Argument;
	union {
		char * String;
		int32_t Int;
		float Float;
	} Value;
};

struct DivFunctions
{
	typedef bool (* CallProc)(uint32_t FunctionId, CallParam * Params);
	typedef bool (* CallProc)(uint32_t FunctionId, CallParam * Params);
	typedef void (* ErrorMessageProc)(char const * Message);
	typedef void (* AssertProc)(char const * Unknown, char const * Message, bool Unknown2);

	void * Unknown0;
	CallProc Call;
	CallProc Query;
	ErrorMessageProc ErrorMessage;
	AssertProc Assert;
};

struct BufferPool
{
	void * VMT;
	uint8_t Unknown;
	uint8_t __Padding[3];
	uint32_t MaxSize;
	uint32_t GrowCount;
	uint32_t Unknown3;
	uint32_t Capacity;
	uint8_t __Padding2[4];
	void * PoolMemoryVMT;
	void * PoolMemory;
	uint32_t CurrentSize;
	uint32_t SomeCounter;
	uint32_t Unknown4;
	uint32_t CurrentPos;
	uint32_t Unknown5;
	uint32_t Unknown6;
	char const * Name;
};

template <typename T>
struct BaseArray
{
	void * VMT;
	T * Buffer;
	uint32_t Capacity;
	uint32_t Size;
	uint32_t Free;


};

struct OsirisInterface;
struct OsirisManager;

enum class FunctionArgumentDirection : uint32_t
{
	In = 1,
	Out = 2
};

struct FunctionArgument
{
	char const * Name;
	ValueType Type;
	uint8_t __Padding[3];
	FunctionArgumentDirection Direction;
};

struct OsirisFunctionHandle
{
	uint32_t Handle;

	OsirisFunctionHandle() : Handle(0) {}
	OsirisFunctionHandle(uint32_t InHandle) : Handle(InHandle) {}

	inline uint8_t GetPart1() const
	{
		return Handle & 7;
	}

	inline uint32_t GetPart2() const
	{
		return (Handle >> 3) & 0x1FFFF;
	}

	inline uint16_t GetFunctionId() const
	{
		return (Handle >> 20) & 0x3FF;
	}

	inline uint32_t GetPart4() const
	{
		return Handle >> 30;
	}
};

struct OsirisFunction
{
	void * VMT;
	char const * Name;
	FunctionArgument * Arguments;
	uint32_t NumArguments;
	uint8_t __Padding[4];
	uint64_t ArgumentSize;
	OsirisFunctionHandle Handle;
	uint8_t __Padding2[4];
	OsirisManager * Manager;
	uint32_t Unknown;
	uint8_t __Padding3[4];
	void * StoryImplementation;
	void * HandlerProc;
	uint64_t Unknown2;
};

struct TypeInfo
{
	char const * Name;
	ValueType Type;
	uint8_t __Padding[1];
	uint16_t Alias;
	uint32_t Unknown;
	uint64_t SizeInBytes;
	uint64_t Unknown2;
	void * TypeFunc1;
	void * TypeFunc2;
};

struct FixedStringMap
{
	uint32_t Capacity;
	uint32_t Unknown;
	void ** Buffer;
	uint32_t Size;
};

struct OsirisManager
{
	void * Allocator;
	OsirisInterface * Interface;
	void * Osiris;
	BaseArray<OsirisFunction *> Functions;
	uint32_t CallbackBufIncrement;
	BaseArray<void *> Objects;
	uint32_t Unknown1;
	FixedStringMap StringMap;
	uint32_t Unknown2;
	std::array<TypeInfo, 16> BuiltinTypes;
	uint32_t Unknown3[2];
	uint64_t Unknown4[3];
	uint64_t NotificationBufferSize;
	BaseArray<char *> NotificationBuffer;
	uint32_t Unknown5;
	uint32_t NotificationBufferClampedSize;
	uint32_t NotificationBufferTotalSize;
	uint64_t Unknown6[3];
};

struct OsirisInterface
{
	void * Osiris;
	OsirisManager * Manager;
	BufferPool ParamBufferPool;
	BufferPool ExecutionContextPool;
	void * Unknown1;
	void * Unknown2;
	void * Unknown3;
};

struct VariableItem
{
	void * ptr;
	uint64_t unused;
};

struct VariableItem2
{
	uint32_t a, b;
	void * strptr;
	uint64_t strpad, str1, str2;
};

struct VariableDb
{
	VariableItem vars[256];
	uint32_t NumVariables;
	uint8_t __Padding[4];
	uint16_t b;
	uint8_t padding[6];
	VariableItem2 * VarsStart;
	VariableItem2 * VarsPtr;
	VariableItem2 * VarsEnd;
};

struct TypeItem
{
	void * ptr;
	uint64_t unused;
};

struct TypeDb
{
	TypeItem vars[1023];
	uint64_t a, b, c, d;
};

template <class T>
struct TArray
{
	uint32_t Size;
	uint32_t __Padding;
	T * Start, *End, *BufEnd;
};

template <class T>
struct Vector
{
	T * Start, * End, * BufEnd;
};

template <class T>
struct ListNode
{
	ListNode<T> * Start, * Prev;
	T Item;
};

template <class T>
struct List
{
	ListNode<T> * Start;
	uint64_t Size;
};

struct Adapter
{

};

struct SomeDbItem
{
	uint64_t Unknown;
	SomeDbItem * Next;
};

union Value
{
	int64_t Int64;
	float Float;
	int32_t Int32;
	char * String;
};

struct String
{
	union {
		char Buf[16];
		char * Ptr;
	};
	uint64_t Length;
	uint64_t BufferLength;
};

struct TValue
{
	Value Val;
	uint32_t Unknown;
	uint32_t __Padding;
	String Str;
};

class TypedValue
{
public:
	virtual bool Serialize(void * SmartBuf);
	virtual ~TypedValue() {};
	virtual bool SetType(ValueType type);
	virtual ValueType GetType();
	virtual bool IsValid();
	virtual void DebugDump(char * log);
	virtual void SetValue(TValue * value);
	virtual TValue * GetValue();
	virtual bool IsVariable();
	virtual void SetOutParam(bool OutParam);
	virtual void SetOutParam2(bool OutParam);
	virtual bool IsOutParam();
	virtual uint8_t Index();
	virtual bool IsUnused();
	virtual bool IsAdapted();

	uint32_t TypeId;
	uint32_t __Padding;
	TValue Value;
};

struct DatabaseParam
{
	uint64_t A;
	uint64_t B;
};

class Fact
{
public:
	virtual ~Fact() {};

	TypedValue * Values;
	uint8_t Size;
	uint8_t __Padding[7];
	uint32_t Unknown;
};

struct TupleData
{
	List<TypedValue> Items;
};

class VirtTuple
{
public:
	virtual ~VirtTuple() {}

	TupleData Data;
};

struct Database
{
	uint64_t A;
	uint64_t B;
	SomeDbItem Items[16];
	uint64_t C;
	List<Fact> Facts;
	Vector<uint32_t> ParamTypes;
	uint8_t NumParams;
	uint8_t __Padding[7];
	Vector<DatabaseParam> OrderedFacts;
};

struct DatabaseDb
{
	TArray<Database *> Db;
};

struct AdapterDb
{
	TArray<Adapter *> Db;
};

struct NodeDb
{
	TArray<class Node *> Db;
};

struct Function
{

};

template <typename ManagerType>
struct Ref
{
	uint32_t Id;
	uint32_t __Padding;
	ManagerType * Manager;
};

typedef Ref<DatabaseDb> DatabaseRef;
typedef Ref<AdapterDb> AdapterRef;
typedef Ref<NodeDb> NodeRef;
typedef Ref<DatabaseDb> DatabaseRef;

class NodeEntryRef
{
public:
	virtual void write(); // TODO
	virtual void read(); // TODO

	NodeRef Node;
	uint32_t EntryPoint;
	uint32_t GoalId;
};

enum AdapterType
{
	AT_EntryPoint = 0,
	AT_Left = 1,
	AT_Right = 2
};

struct NodeVMT
{
	void * Destroy; // 0
	void * GetDatabaseRef; // 8
	void * GetDatabaseRef2; // 10
	void * IsDataNode; // 18
	void * IsValid; // 20
	void * IsProc; // 28
	void * IsPartOfAProc; // 30
	void * GetParent; // 38
	void * SetNextNode; // 40
	void * GetAdapter; // 48
	void * InsertFact; // 50
	void * SelectFacts; // 58
	void * InsertFactLocal; // 60
	void * SelectFactsLocal; // 68
	void * SelectFactsOnAllRefs; // 70
	void * SelectFactsLocalOnAllRefs; // 78
	void * GetLowDatabaseRef; // 80
	void * GetLowDatabase; // 88
	void * GetLowDatabaseFlags; // 90
	void * Save; // 98
	void * DebugDump; // A0
	void * DebugDump2; // A8
	void * SetRightValueIndex_M; // B0
	void * RelUnused; // B8
	void * QueryGetName; // C0
};

typedef char * const (* QueryGetNameProc)();

class Node
{
public:
	virtual ~Node();
	virtual DatabaseRef * GetDatabaseRef(DatabaseRef * Db);
	virtual DatabaseRef * GetDatabaseRef2(DatabaseRef * Db);
	virtual bool IsDataNode();
	virtual bool IsValid(VirtTuple * Values, AdapterRef * Adapter);
	virtual bool IsProc();
	virtual bool IsPartOfAProc();
	virtual NodeRef * GetParent(NodeRef * Node);
	virtual void SetNextNode(NodeEntryRef * Node);
	virtual Adapter * GetAdapter(AdapterType Which);
	virtual void InsertFact(TupleData * Fact);
	virtual void SelectFacts(VirtTuple * Fact, AdapterRef * Adapter, AdapterType Which);
	virtual void InsertFactLocal(TupleData * Fact);
	virtual void SelectFactsLocal(VirtTuple * Fact, AdapterRef * Adapter, AdapterType Which);
	virtual void SelectFactsOnAllRefs(TupleData * Fact);
	virtual void SelectFactsLocalOnAllRefs(TupleData * Fact);
	virtual NodeRef * GetLowDatabaseRef(NodeRef * Node);
	virtual NodeEntryRef * GetLowDatabase(NodeEntryRef * Node);
	virtual uint8_t GetLowDatabaseFlags();
	virtual void Save(void * SmartBuf);
	virtual int DebugDump(char * Buffer);
	virtual int DebugDump2(char * Buffer);
	virtual void SetRightValueIndex_M(int Index);


	uint64_t Unknown1;
	uint64_t Unknown2;
	Function * Function;
	DatabaseRef Database;
};

class TreeNode : public Node
{
public:
	NodeEntryRef Next;
};

class RelNode : public TreeNode
{
public:
	NodeRef Parent;
	AdapterRef Adapter;
	NodeRef RelDatabaseRef;
	NodeEntryRef RelDatabase;
	uint8_t RelDatabaseFlag;
	uint8_t __Padding[7];
};

class RelOpNode : public RelNode
{
public:
	uint8_t LeftValueIndex;
	uint8_t RightValueIndex;
	uint8_t __Padding2[6];
	TypedValue LeftValue;
	TypedValue RightValue;
	uint32_t RelOp;
	uint32_t Unknown;
};

class RuleCallList
{
public:
	virtual ~RuleCallList(); // TODO
	uint64_t Unknown1, Unknown2;
};

struct CallParams
{
	uint64_t Unknown1, Unknown2;
};

struct RuleCall
{
	char * Name;
	CallParams * Params;
	bool Negate;
	uint8_t __Padding[3];
	uint32_t GoalIdOrDebugHook;
};

class RuleNode : public RelNode
{
public:
	uint32_t Line;
	bool IsQuery;
	uint8_t __Padding2[3];
	void * Variables;
	RuleCallList * Calls;
};

class JoinNode : public TreeNode
{
public:
	NodeRef Left;
	NodeRef Right;
	AdapterRef LeftAdapter;
	AdapterRef RightAdapter;
	uint64_t Unknown;
	NodeRef LeftDatabaseRef;
	NodeEntryRef LeftDatabase;
	uint8_t LeftDatabaseFlag;
	uint8_t __Padding1[7];
	NodeRef RightDatabaseRef;
	NodeEntryRef RightDatabase;
	uint8_t RightDatabaseFlag;
	uint8_t __Padding2[7];
};

class AndNode : public JoinNode
{
public:
};

class NotAndNode : public JoinNode
{
public:
};

class DataNode : public Node
{
public:
	uint32_t UsedBy;
};

class ProcNode : public DataNode
{
public:
	uint32_t UsedBy;
};

class DatabaseNode : public DataNode
{
public:
	uint32_t UsedBy;
};

class QueryNode : public Node
{
};

class UserQueryNode : public QueryNode
{
};

class DivQueryNode : public QueryNode
{
};

class InternalQueryNode : public QueryNode
{
};
#pragma pack(pop)


enum class NodeType
{
	Database,
	Proc,
	DivQuery,
	And,
	NotAnd,
	RelOp,
	Rule,
	InternalQuery,
	UserQuery
};

enum DebugFlag
{
	DF_FunctionList = 1 << 16,
	DF_NodeList = 1 << 17,
	DF_CompileTrace = 1 << 18,
	DF_DebugTrace = 1 << 19,
	DF_DebugFacts = 1 << 20,
	DF_LogRuleFailures = 1 << 21,
	DF_ListOrphans = 1 << 22,
	DF_SuppressInitLog = 1 << 23,
	DF_LogFactFailures = 1 << 24,
	DF_LogSuccessfulFacts = 1 << 25,
	DF_LogFailedFacts = 1 << 26,
	DF_SuppressRuleVariables = 1 << 27,
	DF_SuppressParentGoals = 1 << 28,
	DF_SuppressUnknown = 1 << 29,
	DF_SuppressQueryResults = 1 << 30,
	DF_DumpDatabases = 1 << 31
};

typedef void (* COsirisOpenLogFileProc)(void * Osiris, wchar_t const * Path, wchar_t const * Mode);
typedef void (* COsirisCloseLogFileProc)(void * Osiris);
typedef int (* COsirisReadHeaderProc)(void * Osiris, void * OsiSmartBuf, unsigned __int8 * MajorVersion, unsigned __int8 * MinorVersion, unsigned __int8 * BigEndian, unsigned __int8 * Unused, char * StoryFileVersion, unsigned int * DebugFlags);

}
