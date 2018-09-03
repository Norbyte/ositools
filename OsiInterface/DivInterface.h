#pragma once

#include <cstdint>
#include <array>
#include <vector>
#include <set>
#include <map>

namespace osidbg
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
	typedef void (* AssertProc)(bool Successful, char const * Message, bool Unknown2);

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
	ListNode<T> * Next, * Head;
	T Item;
};

template <class T>
struct List
{
	ListNode<T> * Head;
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

struct TypedValueList
{
	uint64_t Size;
	TypedValue Values[1];
};

class TupleVec
{
public:
	virtual ~TupleVec() {};

	// Ptr to (&TypedValueList->Values)
	TypedValue * Values;
	uint8_t Size;
	uint8_t __Padding[7];
	uint32_t Unknown;
};

struct TuplePtrLL
{
	List<TypedValue *> Items;
};

struct TupleLL
{
	struct Item
	{
		uint8_t Index;
		uint8_t __Padding[7];
		TypedValue Value;
	};

	List<Item> Items;
};

class VirtTupleLL
{
public:
	virtual ~VirtTupleLL() {}

	TupleLL Data;
};

struct Database
{
	uint32_t DatabaseId;
	uint32_t __Padding;
	uint64_t B;
	SomeDbItem Items[16];
	uint64_t C;
	List<TupleVec> Facts;
	Vector<uint32_t> ParamTypes;
	uint8_t NumParams;
	uint8_t __Padding2[7];
	Vector<DatabaseParam> OrderedFacts;
};

struct RuleActionParam
{
	void * Unknown;
};

class RuleActionNode
{
public:
	const char * FunctionName;
	List<RuleActionParam> * Arguments;
	bool Not;
	uint8_t __Padding[3];
	int32_t GoalIdOrDebugHook;
};

class RuleActionList
{
public:
	virtual ~RuleActionList() = 0;
	List<RuleActionNode *> Actions;
};

struct Goal
{
	RuleActionList * InitCalls;
	RuleActionList * ExitCalls;
	uint32_t Id;
	uint32_t __Padding1;
	char const * Name;
	uint32_t SubGoalCombination;
	uint32_t __Padding2;
	TArray<uint32_t> ParentGoals;
	TArray<uint32_t> SubGoals;
	uint8_t Flags;
	uint8_t __Padding3[7];
};

struct GoalTreeNode
{
	GoalTreeNode * Left;
	GoalTreeNode * Root;
	GoalTreeNode * Right;
	uint8_t Unknown;
	uint8_t IsLeaf;
	uint8_t __Padding[6];
	uint32_t GoalId;
	uint32_t __Padding2;
	Goal * Goal;
};

struct GoalDb
{
	void * Unknown[2047];
	uint32_t Count;
	uint32_t __Padding;
	GoalTreeNode * Root;

	Goal * FindGoal(uint32_t goalId)
	{
		auto finalTreeNode = Root;
		auto currentTreeNode = Root->Root;
		while (!currentTreeNode->IsLeaf)
		{
			if (currentTreeNode->GoalId >= goalId)
			{
				finalTreeNode = currentTreeNode;
				currentTreeNode = currentTreeNode->Left;
			}
			else
			{
				currentTreeNode = currentTreeNode->Right;
			}
		}

		if (finalTreeNode == Root || goalId < finalTreeNode->GoalId)
			return Root->Goal;
		else
			return finalTreeNode->Goal;
	}
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

enum class EntryPoint : uint32_t
{
	None = 0,
	Left = 1,
	Right = 2
};

class NodeEntryRef
{
public:
	virtual void write() {}; // TODO
	virtual void read() {}; // TODO

	NodeRef Node;
	EntryPoint EntryPoint;
	uint32_t GoalId;
};

struct FunctionParamDesc
{
	uint32_t Type;
	uint32_t Unknown;
};

struct FunctionParamList
{
	void * VMT;
	List<FunctionParamDesc> Params;
};

struct FuncSigOutParamList
{
	uint8_t * Params;
	uint32_t Count;
};

struct FunctionSignature
{
	void * VMT;
	const char * Name;
	FunctionParamList * Params;
	FuncSigOutParamList OutParamList;
	uint32_t Unknown;
};

struct Function
{
	void * VMT;
	uint32_t Line;
	uint32_t Unknown1;
	uint32_t Unknown2;
	uint32_t __Padding;
	FunctionSignature * Signature;
	NodeRef Node;
	uint32_t Type;
	uint32_t Key[4];
	uint32_t Unknown3;
};

struct NodeVMT
{
	using DestroyProc = void (*)(Node * self, bool free);
	using GetDatabaseRefProc = DatabaseRef * (*)(Node * self, DatabaseRef * ref);
	using IsDataNodeProc = bool (*)(Node * self);
	using IsValidProc = bool(*)(Node * self, VirtTupleLL * Values, AdapterRef * Adapter);
	using IsProcProc = bool(*)(Node * self);
	using IsPartOfAProcProc = bool(*)(Node * self);
	using GetParentProc = NodeRef * (*)(Node * self, NodeRef * ref);
	using SetNextNodeProc = void (*)(Node * self, NodeEntryRef * ref);
	using GetAdapterProc = Adapter * (*)(Node * self, EntryPoint which);
	using InsertTupleProc = void (*)(Node * self, TuplePtrLL * tuple);
	using PushDownTupleProc = void(*)(Node * self, VirtTupleLL * tuple, AdapterRef * adapter, EntryPoint entryPoint);
	using TriggerInsertEventProc = void (*)(Node * self, TupleVec * tuple);
	using GetLowDatabaseRefProc = NodeRef * (*)(Node * self, NodeRef * ref);
	using GetLowDatabaseProc = NodeEntryRef * (*)(Node * self, NodeEntryRef * ref);
	using GetLowDatabaseIndirectionProc = int (*)(Node * self);
	using SaveProc = bool (*)(Node * self, void * buf);
	using DebugDumpProc = char * (*)(Node * self, char * buf);
	using SetLineNumberProc = void (*)(Node * self, unsigned int line);
	using GetQueryNameProc = const char * (*)(Node * self);

	DestroyProc Destroy; // 0
	GetDatabaseRefProc GetDatabaseRef; // 8
	GetDatabaseRefProc GetDatabaseRef2; // 10
	IsDataNodeProc IsDataNode; // 18
	IsValidProc IsValid; // 20
	IsProcProc IsProc; // 28
	IsPartOfAProcProc IsPartOfAProc; // 30
	GetParentProc GetParent; // 38
	SetNextNodeProc SetNextNode; // 40
	GetAdapterProc GetAdapter; // 48
	InsertTupleProc InsertTuple; // 50
	PushDownTupleProc PushDownTuple; // 58
	InsertTupleProc DeleteTuple; // 60
	PushDownTupleProc PushDownTupleDelete; // 68
	TriggerInsertEventProc TriggerInsertEvent; // 70
	TriggerInsertEventProc TriggerDeleteEvent; // 78
	GetLowDatabaseRefProc GetLowDatabaseRef; // 80
	GetLowDatabaseProc GetLowDatabase; // 88
	GetLowDatabaseIndirectionProc GetLowDatabaseFlags; // 90
	SaveProc Save; // 98
	DebugDumpProc DebugDump; // A0
	DebugDumpProc DebugDump2; // A8
	SetLineNumberProc SetLineNumber; // B0
	void * RelUnused; // B8
	GetQueryNameProc GetQueryName; // C0
};

class Node
{
public:
	virtual ~Node() = 0;
	virtual DatabaseRef * GetDatabaseRef(DatabaseRef * Db) = 0;
	virtual DatabaseRef * GetDatabaseRef2(DatabaseRef * Db) = 0;
	virtual bool IsDataNode() = 0;
	virtual bool IsValid(VirtTupleLL * Tuple, AdapterRef * Adapter) = 0;
	virtual bool IsProc() = 0;
	virtual bool IsPartOfAProc() = 0;
	virtual NodeRef * GetParent(NodeRef * Node) = 0;
	virtual void SetNextNode(NodeEntryRef * Node) = 0;
	virtual Adapter * GetAdapter(EntryPoint Which) = 0;
	virtual void InsertTuple(TuplePtrLL * Tuple) = 0;
	virtual void PushDownTuple(VirtTupleLL * Tuple, AdapterRef * Adapter, EntryPoint Which, NodeRef Ref) = 0;
	virtual void InsertTuple2(TuplePtrLL * Tuple) = 0;
	virtual void PushDownTuple2(VirtTupleLL * Tuple, AdapterRef * Adapter, EntryPoint Which, NodeRef Ref) = 0;
	virtual void TriggerInsertEvent(TupleVec * Tuple) = 0;
	virtual void TriggerInsertEvent2(TupleVec * Tuple) = 0;
	virtual NodeRef * GetLowDatabaseRef(NodeRef * Node) = 0;
	virtual NodeEntryRef * GetLowDatabase(NodeEntryRef * Node) = 0;
	virtual uint8_t GetLowDatabaseIndirection() = 0;
	virtual bool Save(void * SmartBuf) = 0;
	virtual char * DebugDump(char * Buffer) = 0;
	virtual char * DebugDump2(char * Buffer) = 0;
	virtual void SetLineNumber(int Line) = 0;


	uint32_t Id;
	uint32_t __Padding0;
	NodeDb * NodeDb;
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
	uint8_t RelDatabaseIndirection;
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
	uint32_t __Padding3;

	static inline void HookVMTs(NodeVMT * vmt) {}
};

class RuleNode : public RelNode
{
public:
	uint32_t Line;
	bool IsQuery;
	uint8_t __Padding2[3];
	void * Variables;
	RuleActionList * Calls;

	static inline void HookVMTs(NodeVMT * vmt) {}
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
	uint8_t LeftDatabaseIndirection;
	uint8_t __Padding1[7];
	NodeRef RightDatabaseRef;
	NodeEntryRef RightDatabase;
	uint8_t RightDatabaseIndirection;
	uint8_t __Padding2[7];
};

class AndNode : public JoinNode
{
public:

	static inline void HookVMTs(NodeVMT * vmt) {}
};

class NotAndNode : public JoinNode
{
public:

	static inline void HookVMTs(NodeVMT * vmt) {}
};

struct DataNodeRef
{
	DataNodeRef * Head;
	DataNodeRef * Prev;
	NodeEntryRef Reference;
};

struct DataNodeReferenceList
{
	DataNodeRef * Head;
	uint64_t Count;
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
	None = 0,
	Database = 1,
	Proc = 2,
	DivQuery = 3,
	And = 4,
	NotAnd = 5,
	RelOp = 6,
	Rule = 7,
	InternalQuery = 8,
	UserQuery = 9,
	Max = UserQuery
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
typedef int (* COsirisInitGameProc)(void * Osiris);
typedef int (* COsirisLoadProc)(void * Osiris, void * Buf);
typedef int (* COsirisReadHeaderProc)(void * Osiris, void * OsiSmartBuf, unsigned __int8 * MajorVersion, unsigned __int8 * MinorVersion, unsigned __int8 * BigEndian, unsigned __int8 * Unused, char * StoryFileVersion, unsigned int * DebugFlags);
typedef void (* RuleActionCallProc)(RuleActionNode * Action, void * a1, void * a2, void * a3, void * a4);

}
