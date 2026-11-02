// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors
#pragma once

#include "Nodes/FlowNode_SetBlackboardValues.h"
#include "Types/FlowBlackboardEntry.h"

#include "FlowNode_SetBlackboardValuesOnActor.generated.h"

/**
 * Sets Blackboard key values on a specific actor's Blackboard, where the target actor is itself
 * identified by a key in the flow graph's Blackboard. Use this to configure the Blackboard of
 * an AI actor referenced in the flow graph's own data (e.g. boss actor stored in a Blackboard key).
 * Prefer SetBlackboardValuesV2 for new graphs targeting multiple actors.
 */
UCLASS(DisplayName = "Set Blackboard Values On Actor", meta = (Keywords = "blackboard"))
class AIFLOW_API UFlowNode_SetBlackboardValuesOnActor : public UFlowNode_SetBlackboardValues
{
	GENERATED_BODY()

public:
	UFlowNode_SetBlackboardValuesOnActor();

	// UObject
#if WITH_EDITOR
	virtual void PostInitProperties() override;
#endif // WITH_EDITOR
	// --

protected:
	// UFlowNodeBase
	virtual void UpdateNodeConfigText_Implementation() override;
	// --

public:
	// IFlowBlackboardAssetProvider
#if WITH_EDITOR
	virtual UBlackboardData* GetBlackboardAssetForPropertyHandle(const TSharedPtr<IPropertyHandle>& PropertyHandle) const override;
#endif // WITH_EDITOR
	// --

protected:

	// UFlowNode_SetBlackboardValues
	virtual TArray<AActor*> TryResolveActorsForBlackboard() const override;
	// --

	bool TryAddSpecificActor(TArray<AActor*>& InOutResolvedActors) const;
	bool TryAddBlackboardKeyedActor(TArray<AActor*>& InOutResolvedActors) const;

protected:

	// TODO (update the naming for SpecificActor to array

	// Optional specific actor to use to look for (or inject) the blackboard.
	// If not specified, will use the flow graph's owning actor.
	UPROPERTY(Transient, meta = (DefaultForInputFlowPin, FlowPinType = "Object", DisplayPriority = 1))
	TObjectPtr<AActor> SpecificActor = nullptr;

	// Key in the flow graph's own Blackboard that holds the actor whose Blackboard will be written to.
	UPROPERTY(EditAnywhere, Category = Configuration, DisplayName = "Key for Actor to Set Blackboard Values On", meta = (DisplayPriority = 1))
	FFlowBlackboardEntry BlackboardActorKey;

	static FName INPIN_SpecificActor;
};
