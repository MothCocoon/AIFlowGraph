// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors
#pragma once

#include "AddOns/FlowNodeAddOn.h"
#include "Interfaces/FlowBlackboardAssetProvider.h"
#include "Interfaces/FlowBlackboardInterface.h"

#include "AIFlowNodeAddOn.generated.h"

/**
 * Abstract base for all flow node AddOns that require Blackboard access. Provides
 * IFlowBlackboardInterface and IFlowBlackboardAssetProvider to subclasses. Use this as the
 * base when writing a custom AddOn that reads from or writes to a Blackboard.
 */
UCLASS(Abstract, EditInlineNew, Blueprintable, meta = (Keywords = "Blackboard AddOn"))
class AIFLOW_API UAIFlowNodeAddOn
	: public UFlowNodeAddOn
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

#if WITH_EDITOR
	// IFlowBlackboardAssetProvider
	virtual UBlackboardData* GetBlackboardAssetForPropertyHandle(const TSharedPtr<IPropertyHandle>& PropertyHandle) const override;
	// --
#endif // WITH_EDITOR
};
