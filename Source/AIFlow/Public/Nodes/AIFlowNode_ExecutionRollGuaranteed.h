// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors
#pragma once

#include "AIFlowNode.h"
#include "AIFlowNode_ExecutionRollGuaranteed.generated.h"

/**
 * Fires GuaranteedOut or FailureOut based on a pity-system roll whose success probability climbs by 1/MaximumAttempts
 * each activation, guaranteeing success after MaximumAttempts consecutive failures. Use to add variability to
 * encounter events while ensuring they always eventually occur. The Reset input pin resets the pity counter.
 */
UCLASS(NotBlueprintable, meta = (DisplayName = "Roll Guaranteed", Keywords = "branch random"))
class AIFLOW_API UAIFlowNode_ExecutionRollGuaranteed final : public UAIFlowNode
{
	GENERATED_BODY()

	// Number of attempts before a success is guaranteed. Also sets the initial probability: 1/MaximumAttempts per roll.
	UPROPERTY(EditAnywhere, Category = "RollGuaranteed", meta = (ClampMin = 2, UIMin = 2))
	int32 MaximumAttempts = 2;

	// When true, resets the attempt counter after a successful roll so the pity system starts fresh. When false, the counter keeps climbing until a manual Reset input is fired.
	UPROPERTY(EditAnywhere, Category = "RollGuaranteed")
	bool bResetOnSuccess = false;

private:
	UPROPERTY(SaveGame)
	int32 RollAttempts = 0;

	UPROPERTY(Transient)
	FRandomStream RandomStream;

	UPROPERTY(SaveGame)
	bool bHasSuccessfullyRolled = false;

public:
	UAIFlowNode_ExecutionRollGuaranteed();

#if WITH_EDITOR
	virtual bool CanUserAddOutput() const override { return false; }
#endif

	virtual void OnActivate() override;
	virtual void ExecuteInput(const FName& PinName) override;
	virtual void Cleanup() override;

#if WITH_EDITOR
	virtual FString GetNodeDescription() const override;
	virtual const FFlowAgentDoc& GetAgentDoc() const override;
#endif
	
	static const FName INPIN_Reset;
	static const FName OUTPIN_GuaranteedOut;
	static const FName OUTPIN_FailureOut;
};
