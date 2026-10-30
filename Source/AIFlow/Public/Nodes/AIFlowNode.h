// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#pragma once

#include "Interfaces/FlowBlackboardAssetProvider.h"
#include "Nodes/FlowNode.h"

#include "Interfaces/FlowBlackboardInterface.h"

#include "AIFlowNode.generated.h"

/**
 * Abstract base for all flow nodes that read from or write to a Blackboard. Provides
 * IFlowBlackboardInterface and IFlowBlackboardAssetProvider. Subclass this (or use
 * UAIFlowNodeAddOn for AddOns) whenever a node needs direct Blackboard access.
 */
UCLASS(Abstract, Blueprintable, BlueprintType, DisplayName = "AI Flow Node", meta = (Keywords = "Blackboard"))
class AIFLOW_API UAIFlowNode
	: public UFlowNode
	, public IFlowBlackboardAssetProvider
	, public IFlowBlackboardInterface
{
	GENERATED_BODY()

public:

	// IFlowBlackboardInterface
	virtual UBlackboardComponent* GetBlackboardComponent() const override;
	// --

	// IBlackboardAssetProvider
	virtual UBlackboardData* GetBlackboardAsset() const override;
	// --

	// UFlowNode
	virtual int32 GetRandomSeed() const override;
	// --

#if WITH_EDITOR
	// IFlowBlackboardAssetProvider
	virtual UBlackboardData* GetBlackboardAssetForPropertyHandle(const TSharedPtr<IPropertyHandle>& PropertyHandle) const override;
	// --
#endif // WITH_EDITOR
};
