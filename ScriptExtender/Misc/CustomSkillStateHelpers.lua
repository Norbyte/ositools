
---@meta

---Holds callbacks for a custom skill state.
---All methods are ran before the engine ones.
---Engine return values are used unless PreventDefault is set in the event.
---Methods named with "Action" are related to the action state.
---Methods named with "Behaviour" are related to the behaviour.
---Methods that do not contain those keywords are generally related to visuals.
---@class CustomSkillState
local CustomSkillState = {}

---Passed as the first parameter to all callbacks.
---@class CustomSkillEventParams
---@field StopEvent boolean Prevents further callbacks from being run. Does not prevent the engine method from being executed.
---@field PreventDefault boolean Prevents the engine function from running. Required for your callbacks's return values to be used.

---Values >0 prevent casting.
---@enum CustomSkillState.ValidateTargetResult
local _ = {
    VALID = 0,
    INVALID = 1,
    TOO_CLOSE = 2,
    TOO_FAR = 3,
    TOO_HEAVY = 5,
    BLOCKED = 6,
    INVISIBLE = 7,
    OUT_OF_SIGHT = 8,
    PATH_INTERRUPTED = 9,
    PATH_INTERRUPTED_ROOF = 10,
    CANNOT_SWAP = 11,
}

---@enum CustomSkillState.ValidateTargetSightResult
local _ = {
    OUT_OF_SIGHT = -1,
    HIGH_GROUND_BONUS = 0,
    IN_SIGHT = 1,
    UNKNOWN = 2, -- Purple "+"" signs. Unused?
}

---------------------------------------------
-- SERVER SKILL STATE
---------------------------------------------

---@class EsvCustomSkillState : CustomSkillState
local EsvCustomSkillState = {}

---Called twice when the skill state is owned by the server (created from Osiris, AI, etc.).
---Will not be called for skill states that began on the client.
---@param ev CustomSkillEventParams
---@param skillState EsvSkillState
---@return boolean -- Purpose unknown. Defaults to `false`.
function EsvCustomSkillState:Init(ev, skillState) end

---Never observed to fire.
---@param ev CustomSkillEventParams
---@param skillState EsvSkillState
---@return boolean -- Purpose unknown. Defaults to `false`.
function EsvCustomSkillState:Enter(ev, skillState) end

---Never observed to fire (for Target and Projectile skills, at least)
---@param ev CustomSkillEventParams
---@param skillState EsvSkillState
---@param gameTime number
---@return boolean -- Purpose unknown. Defaults to `false`.
function EsvCustomSkillState:Continue(ev, skillState, gameTime) end

---Fired every tick if the skill state is owned by the server.
---@param ev CustomSkillEventParams
---@param skillState EsvSkillState
---@param teamID integer Team ID of the caster. TODO
function EsvCustomSkillState:Tick(ev, skillState, teamID) end

---Never observed to fire.
---@param ev CustomSkillEventParams
---@param skillState EsvSkillState
function EsvCustomSkillState:Exit(ev, skillState) end

---Called after entering the skill state, if it is owned by the server.
---@param ev CustomSkillEventParams
---@param skillState EsvSkillState
---@return boolean -- Purpose unknown. Defaults to `false`.
function EsvCustomSkillState:EnterBehaviour(ev, skillState) end

---Called every tick if the skill state is owned by the server.
---@param ev CustomSkillEventParams
---@param skillState EsvSkillState
---@return boolean -- Purpose unknown. Defaults to `false`.
function EsvCustomSkillState:ContinueBehaviour(ev, skillState) end

---Called every tick before ContinueBehaviour() if the skill state is owned by the server.
---@param ev CustomSkillEventParams
---@param skillState EsvSkillState
---@param gameTime number TODO
function EsvCustomSkillState:UpdateBehaviour(ev, skillState, gameTime) end

---Called every tick if the skill state is owned by the server.
---@param ev CustomSkillEventParams
---@param skillState EsvSkillState
function EsvCustomSkillState:TickBehaviour(ev, skillState) end -- teamID and deltaTime parameters are not passed possibly due to an oversight.

---Never observed to fire.
---@param ev CustomSkillEventParams
---@param skillState EsvSkillState
---@return boolean -- Purpose unknown. Defaults to `false`.
function EsvCustomSkillState:ExitBehaviour(ev, skillState) end

---Called when entering an the UseSkill state.
---@param ev CustomSkillEventParams
---@param skillState EsvSkillState
---@return boolean -- Purpose unknown. Defaults to `false`.
function EsvCustomSkillState:EnterAction(ev, skillState) end

---Called after TickAction() if the state is owned by the server.
---@param ev CustomSkillEventParams
---@param skillState EsvSkillState
---@return boolean -- Purpose unknown. Defaults to `false`.
function EsvCustomSkillState:ContinueAction(ev, skillState) end

---Called every tick while casting.
---@param ev CustomSkillEventParams
---@param skillState EsvSkillState
function EsvCustomSkillState:UpdateAction(ev, skillState) end -- Time param is not currently passed possibly due to an oversight.

---Called after UpdateAction().
---@param ev CustomSkillEventParams
---@param skillState EsvSkillState
function EsvCustomSkillState:TickAction(ev, skillState) end

---Called when the UseSkill action state finishes.
---@param ev CustomSkillEventParams
---@param skillState EsvSkillState
function EsvCustomSkillState:ExitAction(ev, skillState) end

---Called once after EnterAction(). Purpose unknown.
---@param ev CustomSkillEventParams
---@param skillState EsvSkillState
---@return number -- Purpose unknown. Defaults to `0.0`.
function EsvCustomSkillState:GetTargetDistance(ev, skillState) end

---------------------------------------------
-- CLIENT SKILL STATE
---------------------------------------------

---@class EclCustomSkillState : CustomSkillState
local EclCustomSkillState = {}

---@alias SkillState EclSkillState|EsvSkillState

---Called when the state is entered.
---@param ev CustomSkillEventParams
---@param skillState EclSkillState
---@return boolean -- Purpose unknown. Defaults to `false`.
function EclCustomSkillState:Enter(ev, skillState) end

---Never observed to fire (for Target, at least)
---@param ev CustomSkillEventParams
---@param skillState EclSkillState
---@return boolean -- Purpose unknown. Defaults to `false`.
function EclCustomSkillState:Continue(ev, skillState) end

---Fired every tick.
---Overriding this disables most logic: can't target anything, effects and animations will not play, can't exit the state.
---@param ev CustomSkillEventParams
---@param skillState EclSkillState
---@return boolean -- Purpose unknown. Defaults to `false`.
function EclCustomSkillState:Update(ev, skillState) end

---Fired every tick before Update().
---@param ev CustomSkillEventParams
---@param skillState EclSkillState
function EclCustomSkillState:Tick(ev, skillState) end

---Called when the state exits.
---Overriding this will disable cleanup routines for
---animations and targetting effects.
---@param ev CustomSkillEventParams
---@param skillState EclSkillState
function EclCustomSkillState:Exit(ev, skillState) end

---Fired when certain InputEvents occur, mostly the mouse-related ones (confirm/cancel, left/right click).
---@param ev CustomSkillEventParams
---@param skillState EclSkillState
---@param inputEvent InputEvent
function EclCustomSkillState:OnInputEvent(ev, skillState, inputEvent) end

---Called after Enter().
---@param ev CustomSkillEventParams
---@param skillState EclSkillState
---@return boolean -- Purpose unknown. Defaults to `false`.
function EclCustomSkillState:EnterBehaviour(ev, skillState) end

---Called every tick.
---@param ev CustomSkillEventParams
---@param skillState EclSkillState
---@return boolean -- Purpose unknown. Defaults to `false`.
function EclCustomSkillState:ContinueBehaviour(ev, skillState) end

---Called every tick before ContinueBehaviour().
---Overriding this disables target selection logic and the skill will not be casted with a mouse click.
---@param ev CustomSkillEventParams
---@param skillState EclSkillState
function EclCustomSkillState:UpdateBehaviour(ev, skillState) end

---TODO
---@param ev CustomSkillEventParams
---@param skillState EclSkillState
function EclCustomSkillState:TickBehaviour(ev, skillState) end

---Called when the state exits.
---@param ev CustomSkillEventParams
---@param skillState EclSkillState
---@return boolean -- Purpose unknown. Defaults to `false`.
function EclCustomSkillState:ExitBehaviour(ev, skillState) end

---Called when transitioning from skill-prepare to skill-cast.
---Overriding this effectively stalls the skill-cast forever.
---The method will continue to be called each tick.
---@param ev CustomSkillEventParams
---@param skillState EclSkillState
---@return boolean -- Purpose unknown. Defaults to `false`.
function EclCustomSkillState:EnterAction(ev, skillState) end

---Called after TickAction().
---@param ev CustomSkillEventParams
---@param skillState EclSkillState
---@return boolean -- Purpose unknown. Defaults to `false`.
function EclCustomSkillState:ContinueAction(ev, skillState) end

---Called every tick while casting.
---Overriding this will prevent the state from exiting. Character will be stuck in the idle pose.
---@param ev CustomSkillEventParams
---@param skillState EclSkillState
function EclCustomSkillState:UpdateAction(ev, skillState) end

---Called after UpdateAction().
---@param ev CustomSkillEventParams
---@param skillState EclSkillState
function EclCustomSkillState:TickAction(ev, skillState) end

---Called when the skill-cast finishes.
---@param ev CustomSkillEventParams
---@param skillState EclSkillState
function EclCustomSkillState:ExitAction(ev, skillState) end

---Called when hovering over a possible target, as well as after Enter() and after EnterAction().
---Ex. if the skill cannot target ground, this will not be called when hovering over ground. Same for targeting characters or items.
---I assume this method is used to set the target-related fields within the SkillState instance, however overriding it seemed to not affect anything (for Target-type skills).
---@param ev CustomSkillEventParams
---@param skillState EclSkillState
---@param targetHandle ComponentHandle
---@param position vec3
---@return boolean -- Purpose unknown. Defaults to `false`.
function EclCustomSkillState:GetTarget(ev, skillState, targetHandle, position) end

---Called after Enter() and after EnterAction().
---@param ev CustomSkillEventParams
---@param skillState EclSkillState
---@param position vec3
---@return boolean -- Purpose unknown. Defaults to `false`.
function EclCustomSkillState:GetTargetPosition(ev, skillState, position) end

---Called every tick during skill-prepare.
---Determines the distance displayed under the cursor.
---@param ev CustomSkillEventParams
---@param skillState EclSkillState
---@return number -- In meters. Defaults to `0.0`.
function EclCustomSkillState:GetTargetMoveDistance(ev, skillState) end

---Seems to be highly skill-type-dependant; for Projectile:
---Called to determine the base range of the skill.
---Positions within this range do not show high ground bonus.
---Does not affect the actual max range of the skill.
---@param ev CustomSkillEventParams
---@param skillState EclSkillState
---@return number -- In meters. Defaults to `0.0`.
function EclCustomSkillState:GetTargetRadius(ev, skillState) end

---Called every tick during skill-prepare for a lot of positions, up to 30m away (circular, centered on caster).
---Overriding this overrides the LoS check.
---@param ev CustomSkillEventParams
---@param skillState EclSkillState
---@param targetPosition vec3
---@return CustomSkillState.ValidateTargetSightResult
function EclCustomSkillState:ValidateTargetSight(ev, skillState, targetPosition) end

---Called every tick in skill-prepare for a lot of positions, likely the same ones as ValidateTargetSight.
---@param ev CustomSkillEventParams
---@param skillState EclSkillState
---@param targetHandle ComponentHandle
---@param targetPos vec3
---@param snapToGrid boolean
---@param fillInHeight boolean
---@return CustomSkillState.ValidateTargetResult
function EclCustomSkillState:ValidateTarget(ev, skillState, targetHandle, targetPos, snapToGrid, fillInHeight) end

---Called before ExitAction()
---@param ev CustomSkillEventParams
---@param skillState EclSkillState
function EclCustomSkillState:Finish(ev, skillState) end
