// Copyright 2019 DownToCode. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"

class SWidget;
class IPropertyHandle;

DECLARE_DELEGATE_RetVal_OneParam(FText, FOnGetMotionControllerText, FName)
DECLARE_DELEGATE_RetVal(TArray<FName>, FOnGetMotionControllers)
DECLARE_DELEGATE_TwoParams(FOnMotionControllerChanged, FName, FName)

class SMotionControllerWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMotionControllerWidget)
	{}
		SLATE_EVENT(FOnGetMotionControllerText, OnGetMotionControllerText)
		SLATE_EVENT(FOnMotionControllerChanged, OnMotionControllerChanged)
		SLATE_EVENT(FOnGetMotionControllers, OnGetMotionControllers)
	SLATE_END_ARGS()
public:
	void Construct(const FArguments& InArgs, FName InHandSide);

private:

	TSharedRef<SWidget> BuildMotionControllerMenu();

	FText GetMotionControllerText() const;
	void OnMotionControllerValueTextCommitted(const FText& InNewText, ETextCommit::Type InTextCommit);
	void OnMotionControllerComboValueCommitted(FName InMotionController);

	FOnGetMotionControllerText OnGetMotionControllerText;
	FOnMotionControllerChanged OnMotionControllerChanged;
	FOnGetMotionControllers OnGetMotionControllers;
	FName HandSide;
};