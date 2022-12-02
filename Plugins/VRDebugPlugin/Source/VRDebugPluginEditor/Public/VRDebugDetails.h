// Copyright 2019 DownToCode. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IDetailCustomization.h"
#include "Editor/PropertyEditor/Public/PropertyHandle.h"
#include "Runtime/Slate/Public/Widgets/Input/SCheckBox.h"
#include "Types/SlateEnums.h" // for ETextCommit, ESelectInfo
#include "Misc/Attribute.h"

class IDetailCategoryBuilder;
class IDetailLayoutBuilder;
class IPropertyHandle;
class IPropertyHandleArray;
class IDetailPropertyRow;
class ITableRow;
class STableViewBase;

class FVRDebugDetails : public IDetailCustomization
{
public:
	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IDetailCustomization> MakeInstance();

	/** IDetailCustomization interface */
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailLayout) override;

	TSharedPtr<SCheckBox> CustomCheckBox;
private:
	TArray<TWeakObjectPtr<UObject>> SelectedObjects;
	TSharedPtr<IPropertyHandle> MotionControllerLeftProperty;
	TSharedPtr<IPropertyHandle> MotionControllerRightProperty;
	TSharedPtr<IPropertyHandle> LookForMotionControllersProperty;
	IDetailLayoutBuilder* LayoutBuilder;

	FText GetMotionControllerValueText(FName HandSide) const;
	void OnMotionControllerChanged(FName NewMotionController, FName HandSide);
	TArray<FName> OnGetMotionControllers();
	void CheckboxChanged(ECheckBoxState NewState, TSharedRef<IPropertyHandle> Property);
	bool GetPropertyStateValue(TSharedRef<IPropertyHandle> Property);
	FReply SaveButtonClicked();
	FReply LoadButtonClicked();

};