#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "UnrealMakeJsonTypes.h"
#include "UnrealMakeJsonNode.generated.h"

struct FPropertyChangedEvent;
class FNodeHandlingFunctor;
class FKismetCompilerContext;

UCLASS(BlueprintType, Blueprintable)
class UNREALMAKEJSONEDITOR_API UUnrealMakeJsonNode : public UK2Node
{
	GENERATED_UCLASS_BODY()

public:
	// Begin UEdGraphNode interface.
	virtual void AllocateDefaultPins() override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	// End UEdGraphNode interface.

	// Begin UK2Node interface
	virtual bool IsNodePure() const override { return true; }
	virtual bool ShouldShowNodeProperties() const override { return true; }
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual FText GetMenuCategory() const override;
	virtual FNodeHandlingFunctor* CreateNodeHandler(FKismetCompilerContext& CompilerContext) const override;
	// End UK2Node interface.

protected:
	virtual void CreateProjectionPins(UEdGraphPin* Source);

public:
	UPROPERTY(EditAnywhere, Category = PinOptions)
	TArray<FUnrealJson_NamedType> Inputs;
};
