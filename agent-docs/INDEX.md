# AIFlowGraph Agent Knowledge Base - Index

$KB:flow:guide:aiflowgraph-index

## What this is

The agent-facing knowledge base for the **AIFlowGraph plugin** (`CoreSDK/Plugins/Runtime/ThirdParty/AIFlowGraph/`, uplugin `AIFlow.uplugin`) - a sister plugin to FlowGraph in the same directory. This folder holds node/addon reference articles for AIFlowGraph classes (e.g. `UAIFlowAsset` and its node/addon surface) that have no consuming-project dependency.

## Master index

See the KB master index for the federated layout across all three roots: $KB:kb:guide:master-index

## Placement rule

A node/addon's article lives here **only if it is generic** (owned by this plugin, no consuming-project dependency). A consuming project's own project-specific nodes/addons are documented in that project's own KB root instead - see $KB:kb:guide:master-index.

**Any non-C++/Blueprint authoring language a consuming project's engine fork adds is that project's own addition, not part of upstream CoreSDK/ThirdParty Flow** - authoring guidance for it (generic or project-specific) belongs in the owning project's own KB root instead, never here.

## Layout

- `Concepts/` - `Concept` articles for generic ecosystem semantics specific to the AI Flow layer.

## Concepts

- $KB:flow:concept:flow-blackboard-values - the generic Flow Blackboard value system
  (`UFlowBlackboardEntryValue` + typed subclasses) that AI Flow nodes use to read/write a Behavior
  Tree blackboard. Strictly the Behavior Tree blackboard, not a general graph data store.

## Nodes and AddOns

Query the catalog with `FindFlowNodeTypes` for the blackboard-setting node and the two
blackboard-comparison/configuration addons this plugin owns - narrow with its `AssetClassName`/`Kind`/
`Query` filters and its `sections` parameter ("shape", "brief", "full").

## Conventions

Article template, cross-link tag scheme (`$KB:<domain>:<kind>:<id>`), and locked-block markup are
defined once, KB-wide, by a consuming project's own KB conventions doc - see
$KB:kb:guide:master-index for its location. This root does not depend on any consuming project's
conventions doc directly.

## See also
- $KB:flow:guide:flowgraph-index
- $KB:kb:guide:master-index
