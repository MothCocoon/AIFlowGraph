// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#pragma once

#include "AddOns/AIFlowNodeAddOn.h"
#include "BehaviorTree/Blackboard/BlackboardKeyWrappers.h"
#include "LevelSequence/IFlowPlayLevelSequenceAddOnInterface.h"

#include "FlowNodeAddOn_LevelSequenceActorBinding.generated.h"

/**
 * Add-on for flow nodes that spawn an AFlowLevelSequenceActor. Overrides a named sequence
 * binding with an actor resolved from a blackboard key before Play() is called.
 *
 * Set BindingTag to the tag assigned to the binding in the Sequencer editor
 * (right-click binding -> Tags...). Set BlackboardKey to the blackboard key that holds
 * the actor reference on the flow graph owner actor's UBlackboardComponent.
 */
UCLASS(Blueprintable, BlueprintType, DisplayName = "Level Sequence Actor Binding", meta = (Keywords = "Cutscene LevelSequence Blackboard Encounter AddOn"))
class AIFLOW_API UFlowNodeAddOn_LevelSequenceActorBinding
	: public UAIFlowNodeAddOn
	, public IFlowPlayLevelSequenceAddOnInterface
{
	GENERATED_BODY()

public:

	//~ Begin IFlowPlayLevelSequenceAddOnInterface
	virtual void OnSequencePlayerCreated(AFlowLevelSequenceActor& SequenceActor, AActor* FlowOwner) override;
	//~ End IFlowPlayLevelSequenceAddOnInterface

	/** Tag assigned to the sequence binding in the Sequencer editor (right-click binding -> Tags...). */
	UPROPERTY(EditAnywhere, Category = "Configuration")
	FName BindingTag;

	/** Blackboard key on the flow graph owner's UBlackboardComponent that holds the actor to bind. */
	UPROPERTY(EditAnywhere, Category = "Configuration", DisplayName = "Blackboard Key")
	FBlackboardAnyActorKey ActorKey;

#if WITH_EDITOR
	//~ Begin UFlowNodeBase Interface
	virtual EDataValidationResult ValidateNode() override;
	virtual FString GetNodeDescription() const override;
	//~ End UFlowNodeBase Interface

	virtual const FFlowAgentDoc& GetAgentDoc() const override;
#endif
};
