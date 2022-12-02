// Copyright 2019 DownToCode. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "Input/DragAndDrop.h"
#include "Widgets/Text/STextBlock.h"


class FTreeDragDropOp : public FDragDropOperation
{
public:
	DRAG_DROP_OPERATOR_TYPE(FTreeDragDropOp, FDragDropOperation)

		virtual TSharedPtr<SWidget> GetDefaultDecorator() const override
	{
		return SNew(STextBlock)
			.Text(GetText())
			.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Light.ttf"), 26))
			.ColorAndOpacity(FLinearColor(1, 0, 0, 0));
	}


	FText GetText() const
	{
		return DraggingText;
	}

	static TSharedRef<FTreeDragDropOp> New(FString DraggingText)
	{
		TSharedRef<FTreeDragDropOp> Operation = MakeShareable(new FTreeDragDropOp);
		Operation->DraggingText = FText::FromString(DraggingText);
		Operation->Construct();
		return Operation;
	}

private:
	FText DraggingText;
};
