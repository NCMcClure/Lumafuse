// Copyright 2021 DownToCode. All Rights Reserved.

#include "UMG/DebugWidgetView.h"
#include "VRDebugWidgetFL.h"
#include "VRDebugComponent.h"
#include "Runtime/UMG/Public/Blueprint/WidgetTree.h"
#include "Runtime/Engine/Public/TimerManager.h"

UDebugWidgetView::UDebugWidgetView(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bEmptyTextboxOnClick = true;
}

FReply UDebugWidgetView ::NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnPreviewMouseButtonDown(InGeometry, InMouseEvent);
	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		//Detect textbox clicked
		UPanelWidget* Canvas = Cast<UPanelWidget>(GetRootWidget());
		TArray<UWidget*> Textboxes;
		UVRDebugWidgetFL::GetChildrenOfClass(Canvas, UEditableTextBox::StaticClass(), Textboxes);
		for (UWidget* Widget : Textboxes)
		{
			if (Widget->IsHovered())
			{
				UEditableTextBox* Textbox = Cast<UEditableTextBox>(Widget);

				if (Textbox->IsValidLowLevel())
				{
					ValueOfLastSelectedTextbox = Textbox->GetText();
					LastSelectedTextBox = Textbox;
					if (bEmptyTextboxOnClick)Textbox->SetText(FText());
				}

				SpawnKeyboard();
			}
		}
	}

	return FReply::Unhandled();
}

void UDebugWidgetView::StopLaserPointer(ALaserBeam* LaserBeam)
{
	if (!LaserBeam->IsValidLowLevel()) return;
	LaserBeam->StopReleasePointer();
	FTimerHandle Handle;
	FTimerDelegate TimerCallback;
	TimerCallback.BindLambda([=]
	{
		LaserBeam->StartReleasePointer();
	});
	VRDebugComponent->GetWorld()->GetTimerManager().SetTimer(Handle, TimerCallback, 0.1f, false);
}

void UDebugWidgetView::ResetLastSelectedTextBox()
{
	if (LastSelectedTextBox->IsValidLowLevel())
	{
		if (LastSelectedTextBox->GetText().ToString().IsEmpty())
		{
			LastSelectedTextBox->SetText(ValueOfLastSelectedTextbox);
		}
	}
}
