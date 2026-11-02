// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors
#include "AddOns/FlowNodeAddOn_InjectComponents.h"

#include "Components/ActorComponent.h"
#include "Types/FlowInjectComponentsManager.h"
#include "GameFramework/Actor.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlowNodeAddOn_InjectComponents)

#if WITH_EDITOR
const FFlowAgentDoc& UFlowNodeAddOn_InjectComponents::GetAgentDoc() const
{
	static const FFlowAgentDoc Doc = MakeAgentDoc(
		/*Guidance*/ TEXT("Generic AIFlow-level building block - configure the components to inject via InjectComponentsHelper. For encounter Spawn nodes, prefer the project's specialized InjectComponents subclass when it additionally cleans up injected components on pooling destroys, not just standard AActor destroy."),
		/*Tags*/     { TEXT("addon"), TEXT("spawn"), TEXT("component"), TEXT("inject"), TEXT("base-class") });
	return Doc;
}
#endif // WITH_EDITOR

void UFlowNodeAddOn_InjectComponents::FinishedSpawningActor_Implementation(AActor* SpawnedActor, UFlowNodeBase* SpawningFlowNodeBase)
{
	// It is possible to fail spawning, and we will still get a nullptr call to this function
	// (in case the AddOn is simply counting spawn attempts, etc.)

	if (IsValid(SpawnedActor))
	{
		const TArray<UActorComponent*> ComponentInstances = InjectComponentsHelper.CreateComponentInstancesForActor(*SpawnedActor);
		if (!ComponentInstances.IsEmpty())
		{
			EnsureInjectComponentsManager();

			InjectComponentsManager->InjectComponentsOnActor(*SpawnedActor, ComponentInstances);

			// Inform subclasses that we are monitoring this Actor
			OnStartMonitoringActor(*SpawnedActor);
		}
	}

	Super::FinishedSpawningActor_Implementation(SpawnedActor, SpawningFlowNodeBase);
}

void UFlowNodeAddOn_InjectComponents::UpdateNodeConfigText_Implementation()
{
#if WITH_EDITOR
	FTextBuilder TextBuilder;

	for (UActorComponent* ComponentTemplate : InjectComponentsHelper.ComponentTemplates)
	{
		if (IsValid(ComponentTemplate))
		{
			TextBuilder.AppendLine(FText::FromString(*ComponentTemplate->GetName()));
		}
	}

	for (TSubclassOf<UActorComponent> ComponentClass : InjectComponentsHelper.ComponentClasses)
	{
		if (IsValid(ComponentClass))
		{
			TextBuilder.AppendLine(FText::FromString(*ComponentClass->GetName()));
		}
	}

	SetNodeConfigText(TextBuilder.ToText());
#endif // WITH_EDITOR
}

