// Copyright 2021 DownToCode. All Rights Reserved.

#include "UMG/VRDebugEditableTextBox.h"
#include "Slate/Public/Widgets/Input/SEditableTextBox.h"

void UVRDebugEditableTextBox::SetCaretToEnd()
{
	MyEditableTextBlock->GoTo(ETextLocation::EndOfLine);
}
