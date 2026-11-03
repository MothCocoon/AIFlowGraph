// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR

#include "AIFlowAsset.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Int.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BlackboardData.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "FlowSettings.h"
#include "FlowSubsystem.h"
#include "Nodes/Graph/FlowNode_SubGraph.h"
#include "Policies/FlowStandardPreloadPolicies.h"
#include "Policies/FlowPreloadHelper.h"
#include "Types/FlowInjectComponentsManager.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UnrealType.h"

struct FAIFlowSharedBlackboardGameInstanceAccess : UGameInstance
{
	static void SetWorldContext(UGameInstance& Instance, FWorldContext* Context)
	{
		FWorldContext* UGameInstance::* ContextMember = &FAIFlowSharedBlackboardGameInstanceAccess::WorldContext;
		Instance.*ContextMember = Context;
	}
};

BEGIN_DEFINE_SPEC(FAIFlowSharedBlackboardSpec, "AIFlow.SharedBlackboard",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
	TStrongObjectPtr<UGameInstance> GameInstance;
	TStrongObjectPtr<UFlowSubsystem> Subsystem;
	TStrongObjectPtr<UFlowAsset> RootTemplate;
	TStrongObjectPtr<UFlowAsset> Root;
	TStrongObjectPtr<UAIFlowAsset> Template;
	FInstancedStruct PreviousPreloadPolicy;
	TStrongObjectPtr<UBlackboardData> Blackboard;
	UWorld* World = nullptr;
	AActor* Actor = nullptr;
	FNativeFlowAssetEvent PreviousAdded;
	FNativeFlowAssetEvent PreviousRemoved;
END_DEFINE_SPEC(FAIFlowSharedBlackboardSpec)

void FAIFlowSharedBlackboardSpec::Define()
{
	BeforeEach([this]()
	{
		PreviousAdded = UFlowSubsystem::OnInstancedTemplateAdded;
		PreviousRemoved = UFlowSubsystem::OnInstancedTemplateRemoved;
		UFlowSubsystem::OnInstancedTemplateAdded.Unbind();
		UFlowSubsystem::OnInstancedTemplateRemoved.Unbind();
		PreviousPreloadPolicy = GetMutableDefault<UFlowSettings>()->PreloadPolicy;
		GetMutableDefault<UFlowSettings>()->PreloadPolicy.InitializeAs<FFlowPreloadPolicy_Standard>();
		GameInstance.Reset(NewObject<UGameInstance>(GEngine));
		World = UWorld::CreateWorld(EWorldType::Game, false);
		World->SetGameInstance(GameInstance.Get());
		FWorldContext& Context = GEngine->CreateNewWorldContext(EWorldType::Game);
		Context.OwningGameInstance = GameInstance.Get();
		Context.SetCurrentWorld(World);
		FAIFlowSharedBlackboardGameInstanceAccess::SetWorldContext(*GameInstance.Get(), &Context);
		World->CreateAISystem();
		Subsystem.Reset(NewObject<UFlowSubsystem>(GameInstance.Get()));
		Actor = World->SpawnActor<AActor>();
		RootTemplate.Reset(NewObject<UFlowAsset>());
		Root.Reset(Subsystem->CreateFlowInstance(Actor, RootTemplate.Get()));
		Blackboard.Reset(NewObject<UBlackboardData>());
		FBlackboardEntry Entry;
		Entry.EntryName = TEXT("Sentinel");
		Entry.KeyType = NewObject<UBlackboardKeyType_Int>(Blackboard.Get());
		Blackboard->Keys.Add(Entry);
		Blackboard->UpdateKeyIDs();
		Template.Reset(NewObject<UAIFlowAsset>());
		const FObjectProperty* BlackboardProperty = FindFProperty<FObjectProperty>(UAIFlowAsset::StaticClass(), TEXT("BlackboardAsset"));
		BlackboardProperty->SetObjectPropertyValue_InContainer(Template.Get(), Blackboard.Get());
	});

	AfterEach([this]()
	{
		Subsystem->Deinitialize();
		UFlowSubsystem::OnInstancedTemplateAdded = PreviousAdded;
		UFlowSubsystem::OnInstancedTemplateRemoved = PreviousRemoved;
		GetMutableDefault<UFlowSettings>()->PreloadPolicy = PreviousPreloadPolicy;
		FAIFlowSharedBlackboardGameInstanceAccess::SetWorldContext(*GameInstance.Get(), nullptr);
		GEngine->DestroyWorldContext(World);
		World->DestroyWorld(false);
		Root.Reset();
		RootTemplate.Reset();
		Template.Reset();
		Blackboard.Reset();
		Subsystem.Reset();
		GameInstance.Reset();
	});

	It("assigns the root registry before a subgraph preloads and injects the shared blackboard", [this]()
	{
		TStrongObjectPtr<UAIFlowAsset> ParentTemplate(NewObject<UAIFlowAsset>());
		const FObjectProperty* BlackboardProperty = FindFProperty<FObjectProperty>(UAIFlowAsset::StaticClass(), TEXT("BlackboardAsset"));
		BlackboardProperty->SetObjectPropertyValue_InContainer(ParentTemplate.Get(), Blackboard.Get());
		UFlowNode_SubGraph* Subgraph = NewObject<UFlowNode_SubGraph>(ParentTemplate.Get());
		const FArrayProperty* OutputPinsProperty = FindFProperty<FArrayProperty>(UFlowNode::StaticClass(), TEXT("OutputPins"));
		OutputPinsProperty->ContainerPtrToValuePtr<TArray<FFlowPin>>(Subgraph)->Add(FFlowPreloadHelper::OUTPIN_Preloaded);
		const FSoftObjectProperty* AssetProperty = FindFProperty<FSoftObjectProperty>(UFlowNode_SubGraph::StaticClass(), TEXT("Asset"));
		AssetProperty->SetPropertyValue_InContainer(Subgraph, FSoftObjectPtr(Template.Get()));
		const FGuid NodeGuid = FGuid::NewGuid();
		ParentTemplate->RegisterNode(NodeGuid, Subgraph);
		UAIFlowAsset* Parent = CastChecked<UAIFlowAsset>(Subsystem->CreateFlowInstance(Actor, ParentTemplate.Get()));
		UFlowNode_SubGraph* InstancedNode = CastChecked<UFlowNode_SubGraph>(Parent->GetNodes()[NodeGuid]);
		UAIFlowAsset* Child = Cast<UAIFlowAsset>(Subsystem->GetInstancedSubFlows().FindRef(InstancedNode));
		if (!TestNotNull(TEXT("Subgraph preloaded during parent initialization"), Child))
		{
			return;
		}
		UBlackboardComponent* Component = Child->GetBlackboardComponent();
		TestNotNull(TEXT("Preloaded child injected blackboard"), Component);
		TestEqual(TEXT("Root set before child initialization"), Child->GetOwningRootFlowInstance(), static_cast<UFlowAsset*>(Parent));
		TestEqual(TEXT("Parent uses child's component"), Parent->GetBlackboardComponent(), Component);
		UFlowInjectComponentsManager* RootManager = Parent->FindComponentInjectionManagerInRootFlow();
		if (!TestNotNull(TEXT("Root manager exists after injection"), RootManager))
		{
			return;
		}
		TestEqual(TEXT("Parent and child are users"), RootManager->GetComponentUserCount(Component), 2);
		Child->FinishFlowInstance(EFlowFinishPolicy::Keep);
		Child->DeinitializeInstance();
		TestTrue(TEXT("Child teardown cannot remove parent's component"), IsValid(Component));
		TestEqual(TEXT("Base teardown unregisters the child"),
			RootManager->GetComponentUserCount(Component), 1);
		TestNull(TEXT("Child clears its cached blackboard after base teardown"), Child->GetBlackboardComponent());
		Parent->FinishFlowInstance(EFlowFinishPolicy::Keep);
		Parent->DeinitializeInstance();
		TestFalse(TEXT("Parent releases final subscription"), IsValid(Component));
	});

	It("keeps a creator's blackboard alive for another graph and a later replacement graph", [this]()
	{
		UAIFlowAsset* First = CastChecked<UAIFlowAsset>(Subsystem->CreateFlowInstance(Actor, Template.Get(), FString(), /*bStartReplication=*/true, Root.Get()));
		UBlackboardComponent* Component = First->GetBlackboardComponent();
		if (!TestNotNull(TEXT("Creator injected a blackboard"), Component))
		{
			return;
		}
		Component->SetValueAsInt(TEXT("Sentinel"), 42);
		UAIFlowAsset* Second = CastChecked<UAIFlowAsset>(Subsystem->CreateFlowInstance(Actor, Template.Get(), FString(), /*bStartReplication=*/true, Root.Get()));
		TestEqual(TEXT("Second graph shares the blackboard"), Second->GetBlackboardComponent(), Component);
		First->FinishFlowInstance(EFlowFinishPolicy::Keep);
		First->DeinitializeInstance();
		TestTrue(TEXT("First creator's teardown preserves shared component"), IsValid(Component));
		TestEqual(TEXT("Second graph still has the component"), Second->GetBlackboardComponent(), Component);
		if (!IsValid(Component))
		{
			return;
		}
		TestEqual(TEXT("Shared state survives creator teardown"), Component->GetValueAsInt(TEXT("Sentinel")), 42);
		UAIFlowAsset* Replacement = CastChecked<UAIFlowAsset>(Subsystem->CreateFlowInstance(Actor, Template.Get(), FString(), /*bStartReplication=*/true, Root.Get()));
		TestEqual(TEXT("Replacement shares the same component"), Replacement->GetBlackboardComponent(), Component);
		Second->FinishFlowInstance(EFlowFinishPolicy::Keep);
		Second->DeinitializeInstance();
		TestTrue(TEXT("Replacement keeps the blackboard alive"), IsValid(Component));
		Replacement->FinishFlowInstance(EFlowFinishPolicy::Keep);
		Replacement->DeinitializeInstance();
		TestFalse(TEXT("Last graph tears down the injected blackboard"), IsValid(Component));
	});

	It("finishes without unregistering its blackboard until deinitialization", [this]()
	{
		UAIFlowAsset* Instance = CastChecked<UAIFlowAsset>(Subsystem->CreateFlowInstance(Actor, Template.Get(), FString(), /*bStartReplication=*/true, Root.Get()));
		UBlackboardComponent* Component = Instance->GetBlackboardComponent();
		Instance->FinishFlowInstance(EFlowFinishPolicy::Keep);
		TestTrue(TEXT("Finish retains the component"), IsValid(Component));
		const UFlowInjectComponentsManager* RootManager = Root->FindComponentInjectionManagerInRootFlow();
		if (!TestNotNull(TEXT("Root manager exists after injection"), RootManager))
		{
			return;
		}
		TestEqual(TEXT("Graph remains a user"), RootManager->GetComponentUserCount(Component), 1);
		Instance->DeinitializeInstance();
		TestFalse(TEXT("Deinitialize releases the last user"), IsValid(Component));
	});

	It("separates different blackboard assets on the same actor and component class", [this]()
	{
		TStrongObjectPtr<UBlackboardData> OtherBlackboard(NewObject<UBlackboardData>());
		FBlackboardEntry Entry;
		Entry.EntryName = TEXT("Sentinel");
		Entry.KeyType = NewObject<UBlackboardKeyType_Int>(OtherBlackboard.Get());
		OtherBlackboard->Keys.Add(Entry);
		OtherBlackboard->UpdateKeyIDs();
		TStrongObjectPtr<UAIFlowAsset> OtherTemplate(NewObject<UAIFlowAsset>());
		const FObjectProperty* Property = FindFProperty<FObjectProperty>(UAIFlowAsset::StaticClass(), TEXT("BlackboardAsset"));
		Property->SetObjectPropertyValue_InContainer(OtherTemplate.Get(), OtherBlackboard.Get());
		UAIFlowAsset* First = CastChecked<UAIFlowAsset>(Subsystem->CreateFlowInstance(Actor, Template.Get(), FString(), /*bStartReplication=*/true, Root.Get()));
		UAIFlowAsset* Second = CastChecked<UAIFlowAsset>(Subsystem->CreateFlowInstance(Actor, OtherTemplate.Get(), FString(), /*bStartReplication=*/true, Root.Get()));
		UBlackboardComponent* FirstComponent = First->GetBlackboardComponent();
		UBlackboardComponent* SecondComponent = Second->GetBlackboardComponent();
		if (!TestNotNull(TEXT("First configured component"), FirstComponent)
			|| !TestNotNull(TEXT("Second configured component"), SecondComponent))
		{
			return;
		}
		TestTrue(TEXT("Different blackboard assets do not alias"), FirstComponent != SecondComponent);
		TestEqual(TEXT("Second component has the requested asset"), SecondComponent->GetBlackboardAsset(), OtherBlackboard.Get());
		UAIFlowAsset* Third = CastChecked<UAIFlowAsset>(Subsystem->CreateFlowInstance(Actor, OtherTemplate.Get(), FString(), /*bStartReplication=*/true, Root.Get()));
		TestEqual(TEXT("Same configuration shares its own component"), Third->GetBlackboardComponent(), SecondComponent);
		First->FinishFlowInstance(EFlowFinishPolicy::Keep);
		First->DeinitializeInstance();
		TestFalse(TEXT("First configuration released independently"), IsValid(FirstComponent));
		TestTrue(TEXT("Other configuration preserved"), IsValid(SecondComponent));
	});

	It("discards a blackboard whose initialization fails", [this]()
	{
		UWorld* WorldWithoutAI = UWorld::CreateWorld(EWorldType::Game, false);
		WorldWithoutAI->SetGameInstance(GameInstance.Get());
		TestNull(TEXT("Failure fixture has no AI system"), WorldWithoutAI->GetAISystem());
		AActor* ActorWithoutAI = WorldWithoutAI->SpawnActor<AActor>();
		AddExpectedError(TEXT("Blackboard initialization failed"), EAutomationExpectedErrorFlags::Contains, 1);
		UAIFlowAsset* Instance = CastChecked<UAIFlowAsset>(Subsystem->CreateFlowInstance(ActorWithoutAI, Template.Get()));
		TestNull(TEXT("Failed blackboard is not cached"), Instance->GetBlackboardComponent());
		TestNull(TEXT("Failure leaves no injection manager"), Instance->FindComponentInjectionManagerInRootFlow());
		TestNull(TEXT("Failed component removed from actor"), ActorWithoutAI->FindComponentByClass<UBlackboardComponent>());
		Instance->DeinitializeInstance();
		WorldWithoutAI->DestroyWorld(false);
	});

	It("does not destroy a blackboard supplied by the actor", [this]()
	{
		UBlackboardComponent* Existing = NewObject<UBlackboardComponent>(Actor);
		if (!TestTrue(TEXT("Existing blackboard initialized"), Existing->InitializeBlackboard(*Blackboard.Get())))
		{
			return;
		}
		Existing->RegisterComponent();
		UAIFlowAsset* Instance = CastChecked<UAIFlowAsset>(Subsystem->CreateFlowInstance(Actor, Template.Get(), FString(), /*bStartReplication=*/true, Root.Get()));
		TestEqual(TEXT("Graph borrows actor blackboard"), Instance->GetBlackboardComponent(), Existing);
		Instance->FinishFlowInstance(EFlowFinishPolicy::Keep);
		Instance->DeinitializeInstance();
		TestTrue(TEXT("Borrowed blackboard survives graph teardown"), IsValid(Existing));
		Subsystem->Deinitialize();
		TestTrue(TEXT("Borrowed blackboard survives subsystem teardown"), IsValid(Existing));
	});
}

#endif // WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR
