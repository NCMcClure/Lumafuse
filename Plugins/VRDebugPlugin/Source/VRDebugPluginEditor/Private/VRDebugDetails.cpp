// Copyright 2019 DownToCode. All Rights Reserved.

#include "VRDebugDetails.h"
#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "DetailWidgetRow.h"
#include "Modules/ModuleManager.h"
#include "MotionControllerCustomization.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Runtime/Slate/Public/Widgets/Layout/SScaleBox.h"
#include "Runtime/Slate/Public/Widgets/Text/STextBlock.h"
#include "Runtime/Slate/Public/Widgets/Input/SButton.h"
#include "Runtime/Slate/Public/Widgets/Layout/SBox.h"
#include "VRDebugComponent.h"

#define LOCTEXT_NAMESPACE "VRDebugDetails"

TSharedRef<IDetailCustomization> FVRDebugDetails::MakeInstance()
{
	return MakeShareable(new FVRDebugDetails);
}

void FVRDebugDetails::CustomizeDetails(IDetailLayoutBuilder& DetailLayout)
{
	DetailLayout.GetObjectsBeingCustomized(SelectedObjects);
	
#pragma region SaveLoadButtons
	
	IDetailCategoryBuilder& SaveSettingsCategory= DetailLayout.EditCategory(TEXT("Save Settings"), FText::GetEmpty(), ECategoryPriority::Important);
	LayoutBuilder = &SaveSettingsCategory.GetParentLayout();
	if (SelectedObjects[0]->IsInBlueprint())
	{
		SaveSettingsCategory.AddCustomRow(FText::FromString("Save Settings"))
		.NameContent()
		[
			SNew(SScaleBox)
			.HAlign(EHorizontalAlignment::HAlign_Center)
			.VAlign(EVerticalAlignment::VAlign_Center)
			.IgnoreInheritedScale(true)
			[
				SNew(SBox)
				.WidthOverride(200.0f)
				.HeightOverride(30.0f)
				[
					SNew(SButton)
					.HAlign(EHorizontalAlignment::HAlign_Center)
					.OnClicked(this, &FVRDebugDetails::SaveButtonClicked)
					[
						SNew(SVerticalBox)
						+SVerticalBox::Slot()
						.VAlign(EVerticalAlignment::VAlign_Center)
						[
							SNew(STextBlock)
							.Text(FText::FromString("Save Properties"))
							.Font(FEditorStyle::GetFontStyle(FName("ToolBarButton.LabelFont")))
							.Justification(ETextJustify::Center)
						]
					]
				]
			]
		]
		.ValueContent()
		[
			SNew(SScaleBox)
			.HAlign(EHorizontalAlignment::HAlign_Left)
			.VAlign(EVerticalAlignment::VAlign_Center)
			.IgnoreInheritedScale(true)
			[
				SNew(SBox)
				.WidthOverride(200.0f)
				.HeightOverride(30.0f)
				[
					SNew(SButton)
					.HAlign(EHorizontalAlignment::HAlign_Center)
					.OnClicked(this, &FVRDebugDetails::LoadButtonClicked)
					[
						SNew(SVerticalBox)
						+SVerticalBox::Slot()
						.VAlign(EVerticalAlignment::VAlign_Center)
						[
							SNew(STextBlock)
							.Text(FText::FromString("Load Properties"))
							.Font(FEditorStyle::GetFontStyle(FName("ToolBarButton.LabelFont")))
							.Justification(ETextJustify::Center)
						]
					]
				]
			]
		];
	}
#pragma endregion

#pragma region MotionControllers
	/**
	 * Customise motion controllers names properties
	 */
	TSharedRef<IPropertyHandle> MotionControllerLeftProp = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UVRDebugComponent, MotionControllerLeft));
	if (ensure(MotionControllerLeftProp->IsValidHandle()))
	{
		MotionControllerLeftProperty = MotionControllerLeftProp;
	}

	TSharedRef<IPropertyHandle> MotionControllerRightProp = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UVRDebugComponent, MotionControllerRight));
	if (ensure(MotionControllerRightProp->IsValidHandle()))
	{
		MotionControllerRightProperty = MotionControllerRightProp;
	}

	TSharedRef<IPropertyHandle> LookForMotionControllersProp = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UVRDebugComponent, bLookForMotionControllers));
	if (ensure(LookForMotionControllersProp->IsValidHandle()))
	{
		LookForMotionControllersProperty = LookForMotionControllersProp;
	}

	IDetailCategoryBuilder& MotionControllerPropertyGroup = DetailLayout.EditCategory(*MotionControllerLeftProperty->GetMetaData(TEXT("Category")));
	MotionControllerPropertyGroup.AddCustomRow(LOCTEXT("MotionController", "Motion Controller"))
	.NameContent()
	[
		MotionControllerLeftProperty->CreatePropertyNameWidget()
	]
	.ValueContent()
	[
		SNew(SMotionControllerWidget, TEXT("Left"))
		.OnGetMotionControllerText(this, &FVRDebugDetails::GetMotionControllerValueText)
		.OnMotionControllerChanged(this, &FVRDebugDetails::OnMotionControllerChanged)
		.OnGetMotionControllers(this, &FVRDebugDetails::OnGetMotionControllers)
		.IsEnabled_Lambda([&]()
		{
			if (LookForMotionControllersProperty)
			{
				bool Value;
				LookForMotionControllersProperty->GetValue(Value);
				return !Value;
			}
			else
				return true;
		})
	];

	MotionControllerPropertyGroup.AddCustomRow(LOCTEXT("MotionController", "Motion Controller"))
	.NameContent()
	[
		MotionControllerRightProperty->CreatePropertyNameWidget()
	]
	.ValueContent()
	[
		SNew(SMotionControllerWidget, TEXT("Right"))
		.OnGetMotionControllerText(this, &FVRDebugDetails::GetMotionControllerValueText)
		.OnMotionControllerChanged(this, &FVRDebugDetails::OnMotionControllerChanged)
		.OnGetMotionControllers(this, &FVRDebugDetails::OnGetMotionControllers)
		.IsEnabled_Lambda([&]()
		{
			if (LookForMotionControllersProperty)
			{
				bool Value;
				LookForMotionControllersProperty->GetValue(Value);
				return !Value;
			}
			else
				return true;
		})
	];

	DetailLayout.HideProperty(MotionControllerLeftProperty);
	DetailLayout.HideProperty(MotionControllerRightProperty);
#pragma endregion 
}

FText FVRDebugDetails::GetMotionControllerValueText(FName HandSide) const
{
	if (HandSide == TEXT("Left"))
	{
		FName MotionControllerValue;
		MotionControllerLeftProperty->GetValue(MotionControllerValue);
		return FText::FromName(MotionControllerValue);
	}
	else if (HandSide == TEXT("Right"))
	{
		FName MotionControllerValue;
		MotionControllerRightProperty->GetValue(MotionControllerValue);
		return FText::FromName(MotionControllerValue);
	}
	else
		return FText::FromString("Wrong Hand");
}

void FVRDebugDetails::OnMotionControllerChanged(FName NewMotionController, FName HandSide)
{
	if (HandSide == TEXT("Left"))
	{
		MotionControllerLeftProperty->SetValue(NewMotionController);
	}
	else if (HandSide == TEXT("Right"))
	{
		MotionControllerRightProperty->SetValue(NewMotionController);
	}
}

TArray<FName> FVRDebugDetails::OnGetMotionControllers()
{
	TArray<FName> MotionControllers = UVRDebugComponent::MotionControllerCandidates;
	return MotionControllers;
}

void FVRDebugDetails::CheckboxChanged(ECheckBoxState NewState, TSharedRef<IPropertyHandle> Property)
{
	if (SelectedObjects.Num() > 0)
	{
		UVRDebugComponent* VRDebugComponent = Cast<UVRDebugComponent>(SelectedObjects[0]);
		if (VRDebugComponent)
		{
			for (auto& Elem : VRDebugComponent->PropertiesMap)
			{
				if (Elem.Key == Property->GetProperty()->GetFName().ToString())
				{
					if (NewState == ECheckBoxState::Checked)
					{
						Elem.Value = true;
					}
					else
					{
						Elem.Value = false;
					}
				}
			}
		}
	}
}

bool FVRDebugDetails::GetPropertyStateValue(TSharedRef<IPropertyHandle> Property)
{
	if (SelectedObjects.Num() > 0)
	{
		UVRDebugComponent* VRDebugComponent = Cast<UVRDebugComponent>(SelectedObjects[0]);
		if (VRDebugComponent)
		{
			if (VRDebugComponent->PropertiesMap.Contains(Property->GetProperty()->GetFName().ToString()))
			{
				return VRDebugComponent->PropertiesMap.FindChecked(Property->GetProperty()->GetFName().ToString());
			}
		}
	}

	return false;
}

FReply FVRDebugDetails::SaveButtonClicked()
{
	if (SelectedObjects.Num() > 0)
	{
		UVRDebugComponent* VRDebugComponent = Cast<UVRDebugComponent>(SelectedObjects[0]);
		if (VRDebugComponent)
		{
			VRDebugComponent->SaveProperties();
		}
	}
	return FReply::Handled();
}

FReply FVRDebugDetails::LoadButtonClicked()
{
	for (int32 i = 0; i < SelectedObjects.Num(); i++)
	{		
		UVRDebugComponent* VRDebugComponent = Cast<UVRDebugComponent>(SelectedObjects[i]);
		if (VRDebugComponent)
		{
			VRDebugComponent->LoadProperties();
			LayoutBuilder->ForceRefreshDetails();
		}
	}
	return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE