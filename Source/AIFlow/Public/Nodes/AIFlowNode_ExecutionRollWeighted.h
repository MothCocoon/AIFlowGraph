// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors
#pragma once

#include "AIFlowNode.h"
#include "AIFlowNode_ExecutionRollWeighted.generated.h"

USTRUCT(BlueprintType, meta = (ShowOnlyInnerProperties))
struct FAIFlowNode_RollWeightedOption
{
	GENERATED_BODY()

public:

	FAIFlowNode_RollWeightedOption() = default;
	FAIFlowNode_RollWeightedOption(const FName InOutputName, int32 InWeight);
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RollWeightedOption")
	FName OutputName;
	
	// Weight for this option (higher is better, zero weight will not spawn at all)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 0, UIMin = 0), Category = "RollWeightedOption")
	int32 Weight = 1;
};

/**
 * Fires one of N dynamically-created output pins chosen by a weighted random roll, where each entry in
 * OutputPinOptions defines a pin name and a relative weight. Zero-weight options are never selected. Use to randomly
 * vary encounter scripting paths — e.g. pick between three attack patterns with different likelihoods.
 */
UCLASS(NotBlueprintable, meta = (DisplayName = "Roll Weighted", Keywords = "branch random"))
class AIFLOW_API UAIFlowNode_ExecutionRollWeighted final : public UAIFlowNode
{
	GENERATED_BODY()

public:
	// List of named output options with relative weights. Each entry creates a corresponding output pin. Zero-weight entries are never selected.
	UPROPERTY(EditAnywhere, Category = "RollWeighted", meta = (TitleProperty = "{OutputName} {Weight}"))
	TArray<FAIFlowNode_RollWeightedOption> OutputPinOptions;

	// Random stream to use for all random numbers
	UPROPERTY(Transient)
	FRandomStream RandomStream;
	
	UPROPERTY(Transient)
	int32 TotalWeight = 0;

public:
	UAIFlowNode_ExecutionRollWeighted();

	virtual void OnActivate() override;
	virtual void ExecuteInput(const FName& PinName) override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

	// IFlowContextPinSupplierInterface
	virtual bool SupportsContextPins() const override { return true; }
	virtual TArray<FFlowPin> GetContextOutputs() const override;
	// --

	virtual const FFlowAgentDoc& GetAgentDoc() const override;
#endif

private:
	int32 GetWeightedRandomChoice();
	void CalculateTotalWeight();
};
