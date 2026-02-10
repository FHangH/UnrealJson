#include "UnrealMakeJsonNode.h"
#include "UnrealJsonObject.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphNodeUtils.h"
#include "EdGraphSchema_K2.h"
#include "EdGraphUtilities.h"
#include "KismetCompiler.h"

#define LOCTEXT_NAMESPACE "UnrealMakeJsonNode"

class FKCHandler_UnrealMakeJson : public FNodeHandlingFunctor
{
public:
	explicit FKCHandler_UnrealMakeJson(FKismetCompilerContext& InCompilerContext) : FNodeHandlingFunctor(InCompilerContext)
	{
	}

	virtual void RegisterNets(FKismetFunctionContext& Context, UEdGraphNode* InNode) override
	{
		UUnrealMakeJsonNode* Node = Cast<UUnrealMakeJsonNode>(InNode);
		FNodeHandlingFunctor::RegisterNets(Context, Node);

		check(NULL != Node);
		{
			for (int32 PinIndex = 0; PinIndex < Node->Pins.Num(); ++PinIndex)
			{
				if (UEdGraphPin* Pin = Node->Pins[PinIndex]; EGPD_Output == Pin->Direction)
				{
					// Register Output Term
					FBPTerminal* Term = Context.CreateLocalTerminalFromPinAutoChooseScope(Pin, Context.NetNameMap->MakeValidName(Pin));
					Context.NetMap.Add(Pin, Term);
				}
				else
				{
					// Register Input Term
					if (UEdGraphPin* Net = FEdGraphUtilities::GetNetFromPin(Pin); Context.NetMap.Find(Net))
					{
						// Already mapped
					}
					else
					{
						FBPTerminal* Term = Context.CreateLocalTerminalFromPinAutoChooseScope(Net, Context.NetNameMap->MakeValidName(Net));
						Context.NetMap.Add(Net, Term);
					}
				}
			}
		}
	}

	virtual void Compile(FKismetFunctionContext& Context, UEdGraphNode* Node) override
	{
		const UEdGraphPin* OutputPin = nullptr;

		for (int32 PinIndex = 0; PinIndex < Node->Pins.Num(); ++PinIndex)
		{
			if (const UEdGraphPin* Pin = Node->Pins[PinIndex]; Pin && (EGPD_Output == Pin->Direction))
			{
				OutputPin = Pin;
				break;
			}
		}

		const UClass* Class = UUnrealJsonObject::StaticClass();

		FBPTerminal** TargetTerm = Context.NetMap.Find(OutputPin);
		if (TargetTerm == nullptr)
		{
			return;
		}

		// 1. Call ConstructUnrealJsonObject
		{
			const FName FunctionName = TEXT("ConstructUnrealJsonObject");
			UFunction* FunctionPtr = Class->FindFunctionByName(FunctionName);
			
			// StaticConstruct is static, so we can call it on the class or just as a static function
			// However, in K2 compilation, KCST_CallFunction usually expects a context if not static, 
			// or FunctionContext=nullptr for static calls on UBlueprintFunctionLibrary.
			// ConstructUnrealJsonObject is static on UUnrealJsonObject.
			
			FBlueprintCompiledStatement& Statement = Context.AppendStatementForNode(Node);
			Statement.Type = KCST_CallFunction;
			Statement.FunctionToCall = FunctionPtr;
			Statement.FunctionContext = nullptr; // Static call
			Statement.bIsParentContext = false;
			Statement.LHS = *TargetTerm;
			
			// Argument: WorldContextObject
			// Since we want this to work in various contexts, we might need to pass implicit world context or nullptr.
			// The original VaRest implementation called StaticConstructVaRestJsonObject on VaRestSubsystem.
			// Here we are calling it on UUnrealJsonObject.
			// Let's pass a null literal for WorldContextObject if possible, or try to get self.
			// Actually, let's just create a null term for WorldContextObject argument since it's Optional/Hidden usually.
			
			FBPTerminal* WorldContextTerm = Context.CreateLocalTerminal(ETerminalSpecification::TS_Literal);
			WorldContextTerm->Type.PinCategory = CompilerContext.GetSchema()->PC_Object;
			WorldContextTerm->ObjectLiteral = nullptr; 
			// Wait, the function definition has (UObject* WorldContextObject).
			// If we pass nullptr, it should be fine if the function handles it. 
			// My implementation: return NewObject<UUnrealJsonObject>(); -> NewObject checks Outer. 
			// If Outer is null, it goes to transient package which is fine for JsonObjects usually.
			Statement.RHS.Add(WorldContextTerm);
		}

		// 2. Set Fields
		for (int32 PinIndex = 0; PinIndex < Node->Pins.Num(); ++PinIndex)
		{
			if (UEdGraphPin* Pin = Node->Pins[PinIndex]; Pin && (EGPD_Input == Pin->Direction))
			{
				FBPTerminal** Source = Context.NetMap.Find(FEdGraphUtilities::GetNetFromPin(Pin));

				const FName& FieldName = Pin->PinName;
				const FName& FieldType = Pin->PinType.PinCategory;

				FBPTerminal* FieldNameTerm = Context.CreateLocalTerminal(ETerminalSpecification::TS_Literal);
				FieldNameTerm->Type.PinCategory = CompilerContext.GetSchema()->PC_String;
				FieldNameTerm->SourcePin = Pin;
				FieldNameTerm->Name = FieldName.ToString();
				FieldNameTerm->TextLiteral = FText::FromName(FieldName);

				FBlueprintCompiledStatement& Statement = Context.AppendStatementForNode(Node);
				FName FunctionName;

				const bool bIsArray = Pin->PinType.ContainerType == EPinContainerType::Array;

				if (FieldType == CompilerContext.GetSchema()->PC_Boolean)
				{
					FunctionName = bIsArray ? TEXT("SetBoolArrayField") : TEXT("SetBoolField");
				}
				else if (FieldType == CompilerContext.GetSchema()->PC_Real || FieldType == CompilerContext.GetSchema()->PC_Double)
				{
					FunctionName = bIsArray ? TEXT("SetNumberArrayField") : TEXT("SetNumberField");
				}
				else if (FieldType == CompilerContext.GetSchema()->PC_String)
				{
					FunctionName = bIsArray ? TEXT("SetStringArrayField") : TEXT("SetStringField");
				}
				else if (FieldType == CompilerContext.GetSchema()->PC_Object)
				{
					FunctionName = bIsArray ? TEXT("SetObjectArrayField") : TEXT("SetObjectField");
				}
				else
				{
					continue;
				}

				UFunction* FunctionPtr = Class->FindFunctionByName(FunctionName);
				Statement.Type = KCST_CallFunction;
				Statement.FunctionToCall = FunctionPtr;
				Statement.FunctionContext = *TargetTerm; // Call on the created object
				Statement.bIsParentContext = false;
				Statement.LHS = nullptr;
				Statement.RHS.Add(FieldNameTerm);
				Statement.RHS.Add(*Source);
			}
		}
	}
};

UUnrealMakeJsonNode::UUnrealMakeJsonNode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UUnrealMakeJsonNode::AllocateDefaultPins()
{
	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();

	UClass* Class = UUnrealJsonObject::StaticClass();
	UEdGraphPin* Pin = CreatePin(EGPD_Output, K2Schema->PC_Object, TEXT(""), Class, TEXT("JsonObject"));

	K2Schema->SetPinAutogeneratedDefaultValueBasedOnType(Pin);

	CreateProjectionPins(Pin);
}

FLinearColor UUnrealMakeJsonNode::GetNodeTitleColor() const
{
	// 蔚蓝 RGB: (0.0, 0.75, 1.0)
	return FLinearColor(0.0f, 0.75f, 1.0f);
}

void UUnrealMakeJsonNode::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	const FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	if (PropertyName == GET_MEMBER_NAME_STRING_CHECKED(UUnrealMakeJsonNode, Inputs) ||
		PropertyName == GET_MEMBER_NAME_STRING_CHECKED(FUnrealJson_NamedType, Name) ||
		PropertyName == GET_MEMBER_NAME_STRING_CHECKED(FUnrealJson_NamedType, Type) ||
		PropertyName == GET_MEMBER_NAME_STRING_CHECKED(FUnrealJson_NamedType, bIsArray))
	{
		ReconstructNode();
		GetGraph()->NotifyGraphChanged();
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}

FText UUnrealMakeJsonNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("NodeTitle", "Make Json");
}

void UUnrealMakeJsonNode::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	if (const UClass* ActionKey = GetClass(); ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(NodeSpawner != nullptr);
		ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
	}
}

FText UUnrealMakeJsonNode::GetMenuCategory() const
{
	return LOCTEXT("MenuCategory", "FH|UnrealJson|MakeJson");
}

FNodeHandlingFunctor* UUnrealMakeJsonNode::CreateNodeHandler(class FKismetCompilerContext& CompilerContext) const
{
	return new FKCHandler_UnrealMakeJson(CompilerContext);
}

void UUnrealMakeJsonNode::CreateProjectionPins(UEdGraphPin* Source)
{
	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();
	UClass* Class = UUnrealJsonObject::StaticClass();

	for (const FUnrealJson_NamedType& Input : Inputs)
	{
		FName Type;
		FName SubCategory;
		UObject* Subtype = nullptr;

		switch (Input.Type)
		{
		case EUnrealJson_JsonType::JSON_Bool:
			Type = K2Schema->PC_Boolean;
			break;

		case EUnrealJson_JsonType::JSON_Number:
			Type = K2Schema->PC_Real;
			SubCategory = K2Schema->PC_Double;
			break;

		case EUnrealJson_JsonType::JSON_String:
			Type = K2Schema->PC_String;
			break;

		case EUnrealJson_JsonType::JSON_Object:
			Type = K2Schema->PC_Object;
			Subtype = Class;
			break;
		}

		UEdGraphNode::FCreatePinParams InputPinParams;
		InputPinParams.ContainerType = Input.bIsArray ? EPinContainerType::Array : EPinContainerType::None;
		UEdGraphPin* InputPin = CreatePin(EGPD_Input, Type, SubCategory, Subtype, Input.Name, InputPinParams);

		InputPin->SetSavePinIfOrphaned(false);
	}
}

#undef LOCTEXT_NAMESPACE
