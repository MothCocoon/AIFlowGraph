// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "AddOns/FlowNodeAddOn_LevelSequenceActorBinding.h"

#include "AIFlowLogChannels.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "LevelSequence/FlowLevelSequenceActor.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlowNodeAddOn_LevelSequenceActorBinding)

void UFlowNodeAddOn_LevelSequenceActorBinding::OnSequencePlayerCreated(AFlowLevelSequenceActor& SequenceActor, AActor* FlowOwner)
{
	if (BindingTag.IsNone())
	{
		UE_LOG(LogAIFlow, Error, TEXT("%s: BindingTag is not set - cannot apply actor binding override"), *GetNameSafe(this));
		return;
	}

	if (ActorKey.IsNone())
	{
		UE_LOG(LogAIFlow, Error, TEXT("%s: BlackboardKey is not set - cannot resolve actor for binding"), *GetNameSafe(this));
		return;
	}

	UBlackboardComponent* Blackboard = GetBlackboardComponent();
	if (!IsValid(Blackboard))
	{
		UE_LOG(LogAIFlow, Error, TEXT("%s: No UBlackboardComponent found on flow owner '%s' - cannot resolve actor for binding tag '%s'"),
			*GetNameSafe(this), *GetNameSafe(FlowOwner), *BindingTag.ToString());
		return;
	}

	AActor* ActorToBind = Blackboard->TryGetFirstActorForKey(ActorKey);
	if (!IsValid(ActorToBind))
	{
		UE_LOG(LogAIFlow, Error, TEXT("%s: Blackboard key '%s' did not resolve to a valid actor - cannot apply binding for tag '%s'"),
			*GetNameSafe(this), *ActorKey.BlackboardKey.ToString(), *BindingTag.ToString());
		return;
	}

	UE_LOG(LogAIFlow, Verbose, TEXT("%s: Applying binding override: tag=%s actor=%s sequenceActor=%s"),
		*GetNameSafe(this), *BindingTag.ToString(), *GetNameSafe(ActorToBind), *GetNameSafe(&SequenceActor));

	SequenceActor.AddBinding(BindingTag, ActorToBind);
}

#if WITH_EDITOR
EDataValidationResult UFlowNodeAddOn_LevelSequenceActorBinding::ValidateNode()
{
	EDataValidationResult Result = Super::ValidateNode();

	if (BindingTag.IsNone())
	{
		LogValidationError(TEXT("BindingTag must be set to a tag assigned to the sequence binding in the Sequencer editor."));
		Result = EDataValidationResult::Invalid;
	}

	if (ActorKey.IsNone())
	{
		LogValidationError(TEXT("BlackboardKey must be set to the blackboard key holding the actor to bind."));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}

FString UFlowNodeAddOn_LevelSequenceActorBinding::GetNodeDescription() const
{
	if (BindingTag.IsNone() || ActorKey.IsNone())
	{
		return TEXT("[Not configured]");
	}

	return FString::Printf(TEXT("Bind %s <- BB[%s]"), *BindingTag.ToString(), *ActorKey.BlackboardKey.ToString());
}

const FFlowAgentDoc& UFlowNodeAddOn_LevelSequenceActorBinding::GetAgentDoc() const
{
	static const FFlowAgentDoc Doc = MakeAgentDoc(
		/*Guidance*/ TEXT("BindingTag must exactly match a tag assigned to the target binding in the Sequencer editor (right-click the binding -> Tags), not the binding's display name - a mismatched tag silently leaves the sequence's default binding untouched rather than erroring."),
		/*Tags*/     { TEXT("level-sequence"), TEXT("cutscene"), TEXT("binding") },
		/*Articles*/ {  });
	return Doc;
}
#endif
