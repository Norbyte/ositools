#pragma once

#include <cstdint>
#include <array>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <cassert>
#include <glm/vec3.hpp>
#include <GameDefinitions/BaseTypes.h>

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

struct OsiArgumentValue
{
	ValueType TypeId;
	uint8_t __Padding[7];
	union {
		char const * String;
		int32_t Int32;
		int64_t Int64;
		float Float;
	};

	inline OsiArgumentValue()
		: TypeId(ValueType::None)
	{}

	inline OsiArgumentValue(ValueType type, char const * str)
		: TypeId(type), String(const_cast<char *>(str))
	{}

	inline OsiArgumentValue(float flt)
		: TypeId(ValueType::Real), Float(flt)
	{}

	inline OsiArgumentValue(int32_t i32)
		: TypeId(ValueType::Integer), Int32(i32)
	{}

	inline OsiArgumentValue(int64_t i64)
		: TypeId(ValueType::Integer64), Int64(i64)
	{}

	inline OsiArgumentValue(OsiArgumentValue const & v)
	{
		*this = v;
	}

	inline OsiArgumentValue & operator = (OsiArgumentValue const & v)
	{
		TypeId = v.TypeId;
		switch (v.TypeId) {
		case ValueType::None:
			break;

		case ValueType::Integer:
			Int32 = v.Int32;
			break;

		case ValueType::Integer64:
			Int64 = v.Int64;
			break;

		case ValueType::Real:
			Float = v.Float;
			break;

		case ValueType::String:
		case ValueType::GuidString:
		default:
			String = v.String;
			break;
		}

		return *this;
	}

	std::string ToString() const
	{
		switch (TypeId)
		{
		case ValueType::None:
			return "(Untyped)";

		case ValueType::Integer:
			return std::to_string(Int32);

		case ValueType::Integer64:
			return std::to_string(Int64);

		case ValueType::Real:
			return std::to_string(Float);

		case ValueType::String:
		case ValueType::GuidString:
		case ValueType::CharacterGuid:
		case ValueType::ItemGuid:
		case ValueType::TriggerGuid:
			return String;

		default:
			return "(Unknown)";
		}
	}
};

struct OsiArgumentDesc
{
	OsiArgumentValue Value;
	OsiArgumentDesc * NextParam;

	OsiArgumentDesc()
		: NextParam(nullptr)
	{}

	~OsiArgumentDesc()
	{
		if (NextParam != nullptr) {
			delete NextParam;
		}
	}

	static OsiArgumentDesc * Create(OsiArgumentValue const & v)
	{
		auto desc = new OsiArgumentDesc();
		desc->Value = v;
		return desc;
	}

	inline void Add(OsiArgumentValue const & v)
	{
		if (NextParam == nullptr)
		{
			NextParam = OsiArgumentDesc::Create(v);
		}
		else
		{
			NextParam->Add(v);
		}
	}

	inline uint32_t Count() const
	{
		uint32_t num = 0;
		auto next = this;
		while (next != nullptr) {
			num++;
			next = next->NextParam;
		}

		return num;
	}

	inline OsiArgumentValue const & Get(uint32_t index) const
	{
		auto next = this;
		while (index--) {
			next = next->NextParam;
		}

		return next->Value;
	}

	inline OsiArgumentValue & Get(uint32_t index)
	{
		auto next = this;
		while (index--) {
			next = next->NextParam;
		}

		return next->Value;
	}

	inline OsiArgumentValue const & operator [] (uint32_t index) const
	{
		auto next = this;
		while (index--) {
			next = next->NextParam;
		}

		return next->Value;
	}

	inline OsiArgumentValue & operator [] (uint32_t index)
	{
		auto next = this;
		while (index--) {
			next = next->NextParam;
		}

		return next->Value;
	}

	inline glm::vec3 GetVector(uint32_t index) const
	{
		auto next = this;
		while (index--) {
			next = next->NextParam;
		}

		glm::vec3 vec; 
		vec.x = next->Value.Float;
		vec.y = next->NextParam->Value.Float;
		vec.z = next->NextParam->NextParam->Value.Float;
		return vec;
	}

	inline void SetVector(uint32_t index, glm::vec3 const & vec)
	{
		auto next = this;
		while (index--) {
			next = next->NextParam;
		}

		next->Value.Float = vec.x;
		next->NextParam->Value.Float = vec.y;
		next->NextParam->NextParam->Value.Float = vec.z;
	}
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
	typedef bool (* CallProc)(uint32_t FunctionId, OsiArgumentDesc * Params);
	typedef bool (* CallProc)(uint32_t FunctionId, OsiArgumentDesc * Params);
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
	OsirisFunctionHandle(uint32_t Part1, uint32_t Part2, uint32_t Part3, uint32_t Part4) 
		: Handle((Part1 & 7) | ((Part2 & 0x1FFFF) << 3)
			| ((Part3 & 0x3FF) << 20) | (Part4 << 30))
	{}

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
	ListNode<T> * Next{ nullptr }, * Head{ nullptr };
	T Item;

	ListNode() {}

	ListNode(T const & item)
		: Item(item)
	{}
};

template <class T>
struct List
{
	ListNode<T> * Head{ nullptr };
	uint64_t Size{ 0 };

	void Init()
	{
		auto head = new ListNode<T>();
		Init(head);
	}

	void Init(ListNode<T> * head)
	{
		Head = head;
		Head->Next = head;
		Head->Head = head;
		Size = 0;
	}

	void Insert(T const & value, ListNode<T> * item, ListNode<T> * prev)
	{
		item->Item = value;
		item->Head = Head;
		item->Next = prev->Next;
		prev->Next = item;
		Size++;
	}

	ListNode<T> * Insert(T const & value, ListNode<T> * prev)
	{
		auto item = new ListNode<T>(value);
		item->Head = Head;
		item->Next = prev->Next;
		prev->Next = item;
		Size++;
		return item;
	}

	void Insert(ListNode<T> * item, ListNode<T> * prev)
	{
		item->Head = Head;
		item->Next = prev->Next;
		prev->Next = item;
		Size++;
	}

	ListNode<T> * Insert(ListNode<T> * prev)
	{
		auto item = new ListNode<T>();
		Insert(item, prev);
		return item;
	}
};

template <class T, unsigned TPad>
struct Padded
{
	uint8_t _Pad[TPad];
	T Value;
};

template <class T>
struct Padded<T, 0>
{
	T Value;
};

template <typename TKey, typename TVal, unsigned TKeyPad, unsigned TValPad>
struct TMapNode
{
	TMapNode<TKey, TVal, TKeyPad, TValPad> * Left;
	TMapNode<TKey, TVal, TKeyPad, TValPad> * Root;
	TMapNode<TKey, TVal, TKeyPad, TValPad> * Right;
	bool Color;
	bool IsRoot;
	Padded<TKey, TKeyPad> Key;
	Padded<TVal, TValPad> Value;
};

template <typename TKey, typename TVal, unsigned TKeyPad, unsigned TValPad>
struct TMap
{
	TMapNode<TKey, TVal, TKeyPad, TValPad> * Root;

	TVal * Find(TKey key)
	{
		auto finalTreeNode = Root;
		auto currentTreeNode = Root->Root;
		while (!currentTreeNode->IsRoot)
		{
			if (currentTreeNode->Key.Value < key) {
				currentTreeNode = currentTreeNode->Right;
			} else {
				finalTreeNode = currentTreeNode;
				currentTreeNode = currentTreeNode->Left;
			}
		}

		if (finalTreeNode == Root || key < finalTreeNode->Key.Value)
			return nullptr;
		else
			return &finalTreeNode->Value.Value;
	}

	template <class Visitor>
	void Iterate(Visitor visitor)
	{
		Iterate(Root->Root, visitor);
	}

	template <class Visitor>
	void Iterate(TMapNode<TKey, TVal, TKeyPad, TValPad> * node, Visitor visitor)
	{
		if (!node->IsRoot) {
			visitor(node->Key.Value, node->Value.Value);
			Iterate(node->Left, visitor);
			Iterate(node->Right, visitor);
		}
	}
};



template <class TKey, class TValue>
struct TypeDb
{
	struct HashSlot
	{
		TMap<TKey, TValue, 6, 0> NodeMap;
		void * Unknown;
	};

	TValue * Find(uint32_t hash, TKey key)
	{
		auto & bucket = Hash[hash % 0x3FF];
		return bucket.NodeMap.Find(key);
	}

	HashSlot Hash[1023];
	uint32_t NumItems;
	uint32_t _Pad;
	uint64_t b, c, d;
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

	inline String()
		: Ptr(nullptr), Length(0), BufferLength(15)
	{}
};

struct TValue
{
	Value Val;
	uint32_t Unknown{ 0 };
	uint32_t __Padding;
	String Str;
};

class TypedValue
{
public:
	/*virtual bool Serialize(void * SmartBuf);
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
	virtual bool IsAdapted();*/

	void * VMT{ nullptr };

	uint32_t TypeId{ 0 };
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
	void * VMT;
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
	void * FactsVMT;
	List<TupleVec> Facts;
	Vector<uint32_t> ParamTypes;
	uint8_t NumParams;
	uint8_t __Padding2[7];
	Vector<DatabaseParam> OrderedFacts;
};

class RuleActionArguments
{
public:
	void * VMT;
	List<TypedValue *> Args;
};

class RuleActionNode
{
public:
	const char * FunctionName;
	RuleActionArguments * Arguments;
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

struct GoalDb
{
	void * Unknown[2047];
	uint32_t Count;
	uint32_t __Padding;
	TMap<uint32_t, Goal *, 6, 4> Goals;
};

struct DatabaseDb
{
	TArray<Database *> Db;
};

class VirtTupleLL;
struct AdapterDb;

struct Adapter
{
	uint32_t Id;
	uint32_t __Padding;
	AdapterDb * Db;
	TMap<uint8_t, uint8_t, 0, 0> VarToColumnMaps;
	uint64_t VarToColumnMapCount;
	Vector<int8_t> ColumnToVarMaps;
	VirtTupleLL Constants;
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

	inline uint32_t numOutParams() const
	{
		uint32_t numParams = 0;
		for (uint32_t i = 0; i < (Count >> 3) + ((Count & 7) ? 1 : 0) ; i++) {
			numParams += __popcnt16(Params[i]);
		}

		return numParams;
	}

	inline bool isOutParam(unsigned i) const
	{
		assert(i < Count*8);
		return ((Params[i >> 3] << (i & 7)) & 0x80) == 0x80;
	}
};

struct FunctionSignature
{
	void * VMT;
	const char * Name;
	FunctionParamList * Params;
	FuncSigOutParamList OutParamList;
	uint32_t Unknown;
};

enum class FunctionType : uint32_t
{
	Unknown = 0,
	Event = 1,
	Query = 2,
	Call = 3,
	Database = 4,
	Proc = 5,
	SysQuery = 6,
	SysCall = 7,
	UserQuery = 8
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
	FunctionType Type;
	uint32_t Key[4];
	uint32_t Unknown3;

	inline uint32_t GetHandle() const
	{
		return OsirisFunctionHandle(Key[0], Key[1], Key[2], Key[3]).Handle;
	}
};

// Osiris -> EoCApp function mapping info
struct MappingInfo
{
	const char * Name;
	uint32_t Id;
	uint32_t NumParams;
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
	using CallQueryProc = bool (*)(Node * self, OsiArgumentDesc * args);
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
	union { // B8
		void * RelUnused;
		CallQueryProc CallQuery;
	};
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
	virtual void DeleteTuple(TuplePtrLL * Tuple) = 0;
	virtual void PushDownTupleDelete(VirtTupleLL * Tuple, AdapterRef * Adapter, EntryPoint Which, NodeRef Ref) = 0;
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

// Used by COsiris::Event()
enum class ReturnCode
{
	EventProcessed = 0,
	FunctionUndefined = 1,
	EventIgnored = 2
};

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
typedef bool (* COsirisCompileProc)(void * Osiris, wchar_t const * path, wchar_t const * mode);
typedef int (* COsirisInitGameProc)(void * Osiris);
typedef int (* COsirisLoadProc)(void * Osiris, void * Buf);
typedef bool (* COsirisMergeProc)(void * Osiris, wchar_t * Src);
typedef bool(* COsirisGetFunctionMappingsProc)(void * Osiris, MappingInfo ** mappings, uint32_t * mappingCount);
typedef int (* COsirisDeleteAllDataProc)(void * Osiris, bool DeleteTypes);
typedef int (* COsirisReadHeaderProc)(void * Osiris, void * OsiSmartBuf, unsigned __int8 * MajorVersion, unsigned __int8 * MinorVersion, unsigned __int8 * BigEndian, unsigned __int8 * Unused, char * StoryFileVersion, unsigned int * DebugFlags);
typedef void (* RuleActionCallProc)(RuleActionNode * Action, void * a1, void * a2, void * a3, void * a4);

using OsiTypeDb = TypeDb<STDString, TypeInfo *>;
using FunctionDb = TypeDb<STDString, Function *>;
using ObjectDb = TypeDb<STDString, void *>;

struct OsirisStaticGlobals
{
	VariableDb ** Variables{ nullptr };
	OsiTypeDb ** Types{ nullptr };
	FunctionDb ** Functions{ nullptr };
	ObjectDb ** Objects{ nullptr };
	GoalDb ** Goals{ nullptr };
	AdapterDb ** Adapters{ nullptr };
	DatabaseDb ** Databases{ nullptr };
	NodeDb ** Nodes{ nullptr };
	DebugFlag * DebugFlags{ nullptr };
	void * TypedValueVMT{ nullptr };
};

struct OsirisDynamicGlobals
{
	void * OsirisObject{ nullptr };
	OsirisManager * Manager{ nullptr };
};

}
