// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors
#pragma once

#include "FlowNodeAddOn_InjectComponentsBase.h"

#include "FlowNodeAddOn_InjectComponents.generated.h"

class UActorComponent;
class UFlowInjectComponentsManager;

/**
 * Base AddOn for SpawnActors-type nodes that injects one or more ActorComponents onto each spawned actor immediately
 * after spawning, removing them again when the node is deinitialized.
 */
UCLASS(Blueprintable, meta = (DisplayName = "Inject Components", Keywords = "inject component"))
class AIFLOW_API UFlowNodeAddOn_InjectComponents : public UFlowNodeAddOn_InjectComponentsBase
{
	GENERATED_BODY()

public:

	UFlowNodeAddOn_InjectComponents();

	// IFlowPerSpawnedActorInterface
	void FinishedSpawningActor_Implementation(AActor* SpawnedActor, UFlowNodeBase* SpawningFlowNodeBase) override;
	// --

	// UFlowNodeBase
	virtual void UpdateNodeConfigText_Implementation() override;
#if WITH_EDITOR
	virtual const FFlowAgentDoc& GetAgentDoc() const override;
#endif // WITH_EDITOR
	// --

protected:

	// Defines which component classes to inject onto each spawned actor and how to configure them.
	UPROPERTY(EditAnywhere, Category = Configuration, meta = (ShowOnlyInnerProperties))
	FFlowInjectComponentsHelper InjectComponentsHelper;
};
