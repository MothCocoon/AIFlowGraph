# Flow Blackboard Values (Concept)

$KB:flow:concept:flow-blackboard-values
keywords: blackboard key value ai behavior tree set compare entry data pin flow

## What this is

The generic mechanism AIFlowGraph nodes use to **read and write entries on an actor's Behavior Tree
Blackboard** (`UBlackboardComponent`) from inside a Flow graph. A "blackboard value" here is a small
inline-editable config object that names a target key and carries a typed value to set or compare
against it. This is the "blackboard-key" system in the AI Flow layer.

**This is strictly the Behavior Tree blackboard**, owned by the AIFlowGraph plugin with no project
dependency. It is not a general node-to-node data store; a project's Flow layer may provide its own
per-execution data store that shares the "blackboard"/"data" terminology, and that is a different
system documented in the owning project's root ($KB:kb:guide:master-index).

## The pieces

- **`UFlowBlackboardEntryValue`** (`AIFLOW_API`, abstract, `EditInlineNew`, display name
  "Blackboard Value") - the config base. Its `Key` property (`FFlowBlackboardEntry`) selects which
  blackboard key this value targets.
- **Typed subclasses** - one per Behavior Tree key type: `Bool`, `Int`, `Float`, `Vector`,
  `Rotator`, `Name`, `String`, `Object`, `Class`, `Enum`. Each knows the `UBlackboardKeyType`
  subclass it is built for (`GetSupportedBlackboardKeyType`).

## What it does

- **Set** - `SetOnBlackboardComponent(BlackboardComponent)` writes this value onto the named key.
- **Compare** - `CompareKeyValues(BlackboardComponent, OtherKeyName)` compares this value to a key's
  current value (numeric subclasses can also feed `EArithmeticCompare` comparisons via
  `TryGetNumericalValuesForArithmeticOperation`).
- **Drive from a Data Pin** - `TrySetValueFromInputDataPin(PinName, PinOwnerFlowNode)` lets the value
  come from an incoming Flow **data pin** at runtime instead of being authored inline, and the value
  can be surfaced back out as a data pin property. This is the bridge between the Flow data-pin
  system and the Behavior Tree blackboard.

## When to use / not use

- Use when an AIFlowGraph node needs to set or test an AI blackboard key as part of graph flow.
- Do **not** reach for this as a general node-to-node data store within a graph - use Flow data
  pins. The "blackboard" name is a common source of confusion; this is strictly the Behavior Tree
  blackboard.

## Ecosystem links

- $KB:flow:guide:aiflowgraph-index

## See also

- $KB:kb:guide:master-index
