
#include "UMG/DragScrollBox.h"
#include "UObject/ObjectMacros.h"
#include "Containers/Ticker.h"

#define LOCTEXT_NAMESPACE "UMG"

UDragScrollBoxSlot::UDragScrollBoxSlot(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, Slot(nullptr)
{
	HorizontalAlignment = HAlign_Fill;
	VerticalAlignment = VAlign_Fill;
}

void UDragScrollBoxSlot::BuildSlot(TSharedRef<SDragScrollBox> ScrollBox)
{
#if ENGINE_MAJOR_VERSION == 5
	ScrollBox->AddSlot()
		.Padding(Padding)
		.HAlign(HorizontalAlignment)
		.VAlign(VerticalAlignment)
		.Expose(Slot)
		[
			Content == nullptr ? SNullWidget::NullWidget : Content->TakeWidget()
		];
#else
	Slot = &ScrollBox->AddSlot()
		.Padding(Padding)
		.HAlign(HorizontalAlignment)
		.VAlign(VerticalAlignment)
		[
			Content == nullptr ? SNullWidget::NullWidget : Content->TakeWidget()
		];
#endif
}

void UDragScrollBoxSlot::SetPadding(FMargin InPadding)
{
#if ENGINE_MAJOR_VERSION == 5
	Padding = InPadding;
	if (Slot)
	{
		Slot->SetPadding(InPadding);
	}
#else
	Padding = InPadding;
	if (Slot)
	{
		Slot->Padding(InPadding);
	}
#endif
}

void UDragScrollBoxSlot::SetHorizontalAlignment(EHorizontalAlignment InHorizontalAlignment)
{
	HorizontalAlignment = InHorizontalAlignment;
	if (Slot)
	{
#if ENGINE_MAJOR_VERSION == 5
	Slot->SetHorizontalAlignment(InHorizontalAlignment);
#else
		Slot->HAlign(InHorizontalAlignment);
#endif
	}
}

void UDragScrollBoxSlot::SetVerticalAlignment(EVerticalAlignment InVerticalAlignment)
{
	VerticalAlignment = InVerticalAlignment;
	if (Slot)
	{
#if ENGINE_MAJOR_VERSION == 5
		Slot->SetVerticalAlignment(InVerticalAlignment);
#else
		Slot->VAlign(InVerticalAlignment);
#endif
	}
}

void UDragScrollBoxSlot::SynchronizeProperties()
{
	SetPadding(Padding);
	SetHorizontalAlignment(HorizontalAlignment);
	SetVerticalAlignment(VerticalAlignment);
}

void UDragScrollBoxSlot::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	Slot = nullptr;
}

UDragScrollBox::UDragScrollBox(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, Orientation(Orient_Vertical)
	, ScrollBarVisibility(ESlateVisibility::Visible)
	, ConsumeMouseWheel(EConsumeMouseWheel::WhenScrollingPossible)
	, ScrollbarThickness(5.0f, 5.0f)
	, AlwaysShowScrollbar(false)
	, AllowOverscroll(true)
	, NavigationDestination(EDescendantScrollDestination::IntoView)
	, NavigationScrollPadding(0.0f)
{
	bIsVariable = false;

	SDragScrollBox::FArguments Defaults;
	Visibility = UWidget::ConvertRuntimeToSerializedVisibility(Defaults._Visibility.Get());
	Clipping = EWidgetClipping::ClipToBounds;

	WidgetStyle = *Defaults._Style;
	WidgetBarStyle = *Defaults._ScrollBarStyle;
	bAllowRightClickDragScrolling = true;
}

void UDragScrollBox::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	MyScrollBox.Reset();
}

UClass* UDragScrollBox::GetSlotClass() const
{
	return UDragScrollBoxSlot::StaticClass();
}

void UDragScrollBox::OnSlotAdded(UPanelSlot* InSlot)
{
	// Add the child to the live canvas if it already exists
	if (MyScrollBox.IsValid())
	{
		CastChecked<UDragScrollBoxSlot>(InSlot)->BuildSlot(MyScrollBox.ToSharedRef());
	}
}

void UDragScrollBox::OnSlotRemoved(UPanelSlot* InSlot)
{
	// Remove the widget from the live slot if it exists.
	if (MyScrollBox.IsValid())
	{
		TSharedPtr<SWidget> Widget = InSlot->Content->GetCachedWidget();
		if (Widget.IsValid())
		{
			MyScrollBox->RemoveSlot(Widget.ToSharedRef());
		}
	}
}

TSharedRef<SWidget> UDragScrollBox::RebuildWidget()
{
	MyScrollBox = SNew(SDragScrollBox)
		.Style(&WidgetStyle)
		.ScrollBarStyle(&WidgetBarStyle)
		.Orientation(Orientation)
		.ConsumeMouseWheel(ConsumeMouseWheel)
		.NavigationDestination(NavigationDestination)
		.NavigationScrollPadding(NavigationScrollPadding)
		.OnUserScrolled(BIND_UOBJECT_DELEGATE(FOnUserScrolled, SlateHandleUserScrolled));

	for (UPanelSlot* PanelSlot : Slots)
	{
		if (UDragScrollBoxSlot * TypedSlot = Cast<UDragScrollBoxSlot>(PanelSlot))
		{
			TypedSlot->Parent = this;
			TypedSlot->BuildSlot(MyScrollBox.ToSharedRef());
		}
	}

	return MyScrollBox.ToSharedRef();
}

void UDragScrollBox::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	MyScrollBox->SetScrollOffset(DesiredScrollOffset);
	MyScrollBox->SetOrientation(Orientation);
	MyScrollBox->SetScrollBarVisibility(UWidget::ConvertSerializedVisibilityToRuntime(ScrollBarVisibility));
	MyScrollBox->SetScrollBarThickness(ScrollbarThickness);
	MyScrollBox->SetScrollBarAlwaysVisible(AlwaysShowScrollbar);
	MyScrollBox->SetAllowOverscroll(AllowOverscroll ? EAllowOverscroll::Yes : EAllowOverscroll::No);
	MyScrollBox->SetScrollBarRightClickDragAllowed(bAllowRightClickDragScrolling);
}

float UDragScrollBox::GetScrollOffset() const
{
	if (MyScrollBox.IsValid())
	{
		return MyScrollBox->GetScrollOffset();
	}

	return 0;
}

float UDragScrollBox::GetViewOffsetFraction() const
{
	if (MyScrollBox.IsValid())
	{
		return MyScrollBox->GetViewOffsetFraction();
	}

	return 0;
}

void UDragScrollBox::SetScrollOffset(float NewScrollOffset)
{
	DesiredScrollOffset = NewScrollOffset;

	if (MyScrollBox.IsValid())
	{
		MyScrollBox->SetScrollOffset(NewScrollOffset);
	}
}

void UDragScrollBox::ScrollToStart()
{
	if (MyScrollBox.IsValid())
	{
		MyScrollBox->ScrollToStart();
	}
}

void UDragScrollBox::ScrollToEnd()
{
	if (MyScrollBox.IsValid())
	{
		MyScrollBox->ScrollToEnd();
	}
}

void UDragScrollBox::ScrollWidgetIntoView(UWidget* WidgetToFind, bool AnimateScroll, EDescendantScrollDestination InScrollDestination)
{
	TSharedPtr<SWidget> SlateWidgetToFind;
	if (WidgetToFind)
	{
		SlateWidgetToFind = WidgetToFind->GetCachedWidget();
	}

	if (MyScrollBox.IsValid())
	{
		// NOTE: Pass even if null! This, in effect, cancels a request to scroll which is necessary to avoid warnings/ensures 
		//       when we request to scroll to a widget and later remove that widget!
		MyScrollBox->ScrollDescendantIntoView(SlateWidgetToFind, AnimateScroll, InScrollDestination);
	}
}

void UDragScrollBox::PostLoad()
{
	Super::PostLoad();

#if ENGINE_MAJOR_VERSION == 5
	if (GetLinkerUEVersion() < VER_UE4_DEPRECATE_UMG_STYLE_ASSETS)
#else
	if (GetLinkerUE4Version() < VER_UE4_DEPRECATE_UMG_STYLE_ASSETS)
#endif
	{
		if (Style_DEPRECATED != nullptr)
		{
			const FScrollBoxStyle* StylePtr = Style_DEPRECATED->GetStyle<FScrollBoxStyle>();
			if (StylePtr != nullptr)
			{
				WidgetStyle = *StylePtr;
			}

			Style_DEPRECATED = nullptr;
		}

		if (BarStyle_DEPRECATED != nullptr)
		{
			const FScrollBarStyle* StylePtr = BarStyle_DEPRECATED->GetStyle<FScrollBarStyle>();
			if (StylePtr != nullptr)
			{
				WidgetBarStyle = *StylePtr;
			}

			BarStyle_DEPRECATED = nullptr;
		}
	}
}

void UDragScrollBox::SetOrientation(EOrientation NewOrientation)
{
	Orientation = NewOrientation;

	if (MyScrollBox.IsValid())
	{
		MyScrollBox->SetOrientation(Orientation);
	}
}

void UDragScrollBox::SetScrollBarVisibility(ESlateVisibility NewScrollBarVisibility)
{
	ScrollBarVisibility = NewScrollBarVisibility;

	if (MyScrollBox.IsValid())
	{
		switch (ScrollBarVisibility)
		{
		case ESlateVisibility::Collapsed:				MyScrollBox->SetScrollBarVisibility(EVisibility::Collapsed); break;
		case ESlateVisibility::Hidden:					MyScrollBox->SetScrollBarVisibility(EVisibility::Hidden); break;
		case ESlateVisibility::HitTestInvisible:		MyScrollBox->SetScrollBarVisibility(EVisibility::HitTestInvisible); break;
		case ESlateVisibility::SelfHitTestInvisible:	MyScrollBox->SetScrollBarVisibility(EVisibility::SelfHitTestInvisible); break;
		case ESlateVisibility::Visible:					MyScrollBox->SetScrollBarVisibility(EVisibility::Visible); break;
		}
	}
}

void UDragScrollBox::SetScrollbarThickness(const FVector2D& NewScrollbarThickness)
{
	ScrollbarThickness = NewScrollbarThickness;

	if (MyScrollBox.IsValid())
	{
		MyScrollBox->SetScrollBarThickness(ScrollbarThickness);
	}
}

void UDragScrollBox::SetAlwaysShowScrollbar(bool NewAlwaysShowScrollbar)
{
	AlwaysShowScrollbar = NewAlwaysShowScrollbar;

	if (MyScrollBox.IsValid())
	{
		MyScrollBox->SetScrollBarAlwaysVisible(AlwaysShowScrollbar);
	}
}

void UDragScrollBox::SetAllowOverscroll(bool NewAllowOverscroll)
{
	AllowOverscroll = NewAllowOverscroll;

	if (MyScrollBox.IsValid())
	{
		MyScrollBox->SetAllowOverscroll(AllowOverscroll ? EAllowOverscroll::Yes : EAllowOverscroll::No);
	}
}

void UDragScrollBox::SlateHandleUserScrolled(float CurrentOffset)
{
	OnUserScrolled.Broadcast(CurrentOffset);
}

#if WITH_EDITOR

const FText UDragScrollBox::GetPaletteCategory()
{
	return LOCTEXT("Panel", "Panel");
}

void UDragScrollBox::OnDescendantSelectedByDesigner(UWidget* DescendantWidget)
{
	UWidget* SelectedChild = UWidget::FindChildContainingDescendant(this, DescendantWidget);
	if (SelectedChild)
	{
		ScrollWidgetIntoView(SelectedChild, true);

		if (TickHandle.IsValid())
		{
#if ENGINE_MAJOR_VERSION == 5
			FTSTicker::GetCoreTicker().RemoveTicker(TickHandle);
#else
			FTicker::GetCoreTicker().RemoveTicker(TickHandle);
#endif
			TickHandle.Reset();
		}
	}
}

void UDragScrollBox::OnDescendantDeselectedByDesigner(UWidget* DescendantWidget)
{
	if (TickHandle.IsValid())
	{
#if ENGINE_MAJOR_VERSION == 5
		FTSTicker::GetCoreTicker().RemoveTicker(TickHandle);
#else
		FTicker::GetCoreTicker().RemoveTicker(TickHandle);
#endif
		TickHandle.Reset();
	}

	// because we get a deselect before we get a select, we need to delay this call until we're sure we didn't scroll to another widget.
#if ENGINE_MAJOR_VERSION == 5
	TickHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([=](float) -> bool
#else
	TickHandle = FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([=](float) -> bool
#endif
	{
		QUICK_SCOPE_CYCLE_COUNTER(STAT_UScrollBox_ScrollToStart_LambdaTick);
		this->ScrollToStart();
		return false;
	}));
}

#endif

/////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE
