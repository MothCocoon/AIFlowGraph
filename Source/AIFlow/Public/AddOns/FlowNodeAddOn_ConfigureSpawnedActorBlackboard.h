// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors
#pragma once

#include "AIFlowActorBlackboardHelper.h"
#include "Interfaces/FlowPerSpawnedActorInterface.h"
#include "FlowNodeAddOn_InjectComponentsBase.h"

#include "FlowNodeAddOn_ConfigureSpawnedActorBlackboard.generated.h"

class UFlowInjectComponentsManager;

/**
 * AddOn for SpawnActors-type nodes that writes Blackboard key values onto each spawned actor's
 * Blackboard immediately after the actor finishes spawning. Supports per-actor option sets for
 * assigning different values to different actors. Use this instead of a separate
 * SetBlackboardValues node when configuring actors at spawn time.
 */
UCLASS(DisplayName = "Configure Spawned Actor Blackboard", meta = (Keywords = "blackboard"))
class AIFLOW_API UFlowNodeAddOn_ConfigureSpawnedActorBlackboard : public UFlowNodeAddOn_InjectComponentsBase
{
	GENERATED_BODY()

public:

	// IFlowPerSpawnedActorInterface
	virtual void FinishedSpawningActor_Implementation(AActor* SpawnedActor, UFlowNodeBase* SpawningNodeOrAddOn) override;
	// --

protected:
	// UFlowNodeBase
	virtual void UpdateNodeConfigText_Implementation() override;
	// --

public:
	// IBlackboardAssetProvider
	virtual UBlackboardData* GetBlackboardAsset() const override;
	// --

	// IFlowBlackboardAssetProvider
#if WITH_EDITOR
	virtual UBlackboardData* GetBlackboardAssetForPropertyHandle(const TSharedPtr<IPropertyHandle>& PropertyHandle) const override;
#endif // WITH_EDITOR
	// --

protected:

	virtual UBlackboardComponent* TryEnsureBlackboardComponentToApplyTo(AActor* SpawnedActor, UFlowNodeBase* SpawningNodeOrAddOn);

protected:

	// Optional explicit Blackboard asset expected on the spawned actors. Used to validate and locate the correct BlackboardComponent when actors may have multiple.
	UPROPERTY(EditAnywhere, Category = Configuration, DisplayName = "Expected Blackboard Asset for Actors", meta = (DisplayPriority = 1))
	TObjectPtr<UBlackboardData> ExpectedBlackboardData = nullptr;

	// Blackboard key/value pairs to write onto every spawned actor's Blackboard.
	UPROPERTY(EditAnywhere, Category = Configuration, meta = (DisplayPriority = 3))
	FAIFlowConfigureBlackboardOption EntriesForEveryActor;

	// Where to search for the desired blackboard: the Actor, their Controller (if Pawn) or both.
	UPROPERTY(EditAnywhere, Category = Configuration, DisplayName = "Blackboard Component Search Rule", meta = (DisplayPriority = 2))
	EActorBlackboardSearchRule SearchRule = EActorBlackboardSearchRule::ActorAndController;

	// Injection rule, if desired blackboard is not found
	UPROPERTY(EditAnywhere, Category = Configuration, DisplayName = "Blackboard Injection Rule", meta = (DisplayPriority = 2))
	EActorBlackboardInjectRule InjectRule = EActorBlackboardInjectRule::InjectOntoActorIfMissing;

	UPROPERTY(EditAnywhere, Category = Configuration, DisplayName = "Per-Actor Options Assignment Method", meta = (DisplayPriority = 4))
	EPerActorOptionsAssignmentMethod PerActorOptionsAssignmentMethod = EPerActorOptionsAssignmentMethod::InOrderWithWrapping;

	// Optional per-actor Blackboard entry sets applied in order (or round-robin) across spawned actors, allowing different values per actor.
	UPROPERTY(EditAnywhere, Category = Configuration, DisplayName = "Per-Actor Options", meta = (DisplayPriority = 4))
	TArray<FAIFlowConfigureBlackboardOption> PerActorOptions;

	// Helper struct that shared functionality for manipulating Actor blackboards
	UPROPERTY(EditAnywhere, Category = Configuration, meta = (ShowOnlyInnerProperties, DisplayPriority = 4))
	FAIFlowActorBlackboardHelper ActorBlackboardHelper;

#if WITH_EDITOR
public:
	virtual const FFlowAgentDoc& GetAgentDoc() const override;
#endif
};
