#pragma once

#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/Enumerations.h>

BEGIN_SE()

struct InputValue
{
	int16_t DeviceId;
	__int16 field_2;
	InputModifier Modifiers;
	uint8_t field_5[3];
	uint8_t State;
	uint8_t field_9;
	uint8_t field_A;
	uint8_t field_B;
};


struct InputDevice
{
	__int64 field_0;
	__int64 field_8;
	__int16 DeviceId;
	__int64 field_18;
	__int64 field_20;
};


struct InputEventDesc
{
	int EventID;
	uint8_t Flags1;
	int Flags2;
	FixedString CategoryName;
	__int64 field_18;
	__int64 field_20;
	__int64 field_28;
	__int64 field_30;
	const char* EventName;
	TranslatedString EventDesc;
	int field_E8;
};

struct alignas(4) InputRaw
{
	int16_t InputRawType;
	int16_t _Padding;
	InputDeviceId DeviceId;
	int16_t _Padding2;
};

struct InputRawChange
{
	InputRaw Input;
	InputValue Value;
};


struct InjectInputData : public InputRawChange
{
};

struct InjectTextData
{
	int16_t DeviceId;
	char Text[64];
};

struct InjectDeviceEvent
{
	uint32_t EventId;
	uint16_t DeviceId;
};

struct InputEventText
{
	int PlayerId;
	char const* Text;
	uint64_t TextLength;
};

struct InputEvent
{
	int EventId;
	int InputPlayerIndex;
	InputDeviceId DeviceId;
	InputValue OldValue;
	InputValue NewValue;
	InputType Type;
	bool WasPreferred;
};

struct FireEventDesc
{
	InputEventDesc* EventDesc;
	int32_t PlayerIndex;
	InputEvent Event;
	int16_t DeviceId;
};


struct HoldRepeatEventDesc
{
	__int64 field_0;
	__int64 field_8;
	__int64 field_10;
	int field_18;
	__int64 field_20;
	int field_28;
	InputEvent Event;
	__int16 field_58;
	int field_5C;
};

struct InputEventListener
{
	virtual ~InputEventListener() = 0;
	virtual int GetInputListenerPriority() = 0;
	virtual uint64_t GetInputListenerFilter() = 0;
	virtual const char* GetInputListenerName() = 0;
	virtual bool IsModal() = 0;
	virtual bool InTextInput(int playerIndex) = 0;
	virtual uint16_t* OnInputEvent(uint16_t* eventRetVal, InputEvent* inputEvent) = 0;
	virtual void OnInputModifierChangedEvent(bool, bool, bool, bool) = 0;
	virtual uint16_t* OnInputEvent(uint16_t* eventRetVal, InputEventText* inputEvent) = 0;
	virtual uint16_t* OnUnlinkedInput(uint16_t* eventRetVal, uint16_t inputDeviceId) = 0;
};

struct InputListenerGroup
{
	int RefCount;
	ObjectSet<InputEventListener*> Listeners;
	ObjectSet<InputEventListener*> Listeners2;
	bool IsTraversing;
};

struct InputBinding : public InputRaw
{
	InputModifier Modifiers;
	uint8_t Modifiers2[3];
};

struct InputBindingDesc
{
	int32_t PlayerIndex;
	int32_t field_4;
	InputBinding Binding;
	int32_t field_14;
	bool field_18;
};

struct InputScheme
{
	struct Binding
	{
		int PlayerId;
		__int64 field_8;
		InputBinding Binding;
	};
	
	struct BindingSet
	{
		ObjectSet<InputBindingDesc> Bindings;
		bool Initialized;
	};

	std::array<BindingSet, 173>* Bindings;
	std::array<RefMap<int32_t, Array<InputBinding>>, 4> PerPlayerBindings;
	Map<uint16_t, int32_t> DeviceIdToPlayerId;
	std::array<ObjectSet<void*>, 4> InputDeviceLists;
	std::array<Map<uint64_t, uint64_t>, 4> InputDeviceSomeMaps;
};

struct InputValueSet
{
	struct InputValueDefn
	{
		float field_0;
		int field_4;
		uint8_t field_8;
	};

	std::array<InputValueDefn, 173> Inputs;
	bool Initialized;
};

struct InputManager
{
	using GetInstanceProc = InputManager * ();

	struct HoldRepeatEvent
	{
		ObjectSet<FireEventDesc> FireEvents;
		ObjectSet<FireEventDesc> ReleaseEvents;
		ObjectSet<FireEventDesc> ValueChangeEvents;
		ObjectSet<FireEventDesc> HoldEvents;
		ObjectSet<FireEventDesc> RepeatEvents;
		ObjectSet<HoldRepeatEventDesc> PressEvents;
	};

	RefMap<int32_t, HoldRepeatEvent> PerPlayerHoldRepeatEvents;
	Array<InputValueSet*> InputValueSets;
	RefMap<int32_t, InputEventDesc> InputDefinitions;
	InputScheme InputScheme;
	InputModifier PressedModifiers;
	double LastUpdateTime;
	bool ControllerAllowKeyboardMouseInput;
	Map<int32_t, void*> field_198;
	InputListenerGroup InputListenerGroup1;
	InputListenerGroup InputListenerGroup;
	ObjectSet<InputRaw> RawInputs;
	std::array<int, 4> PlayerDevices;
	std::array<int16_t, 4> PlayerDeviceIDs;
	InputScheme::Binding CurrentRemap;
	ObjectSet<InputRaw> RawInputs2;
#if !defined(OSI_EOCAPP)
	std::array<ObjectSet<uint32_t>, 12> RawInputTypes;
	ObjectSet<InjectTextData> TextInjects;
#endif
	ObjectSet<InjectInputData> InputInjects;
	ObjectSet<InjectDeviceEvent> DeviceEventInjects;
	float field_4A8[4][4];
	std::array<InputDevice, 4> PerDeviceData;
};

END_SE()
