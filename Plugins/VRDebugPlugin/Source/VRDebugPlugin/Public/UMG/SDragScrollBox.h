// Copyright 2021 DownToCode. All Rights Reserved.

#pragma once
#if ENGINE_MAJOR_VERSION == 5
#include "CoreMinimal.h"
#include "Layout/Visibility.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Layout/Geometry.h"
#include "Styling/SlateColor.h"
#include "Input/CursorReply.h"
#include "Input/Reply.h"
#include "Input/NavigationReply.h"
#include "Widgets/SWidget.h"
#include "Widgets/SPanel.h"
#include "Widgets/SBoxPanel.h"
#include "Layout/Children.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "Styling/SlateTypes.h"
#include "Styling/CoreStyle.h"
#include "Widgets/Layout/SScrollBar.h"
#include "Framework/Layout/InertialScrollManager.h"
#include "Framework/Layout/Overscroll.h"

class FPaintArgs;
class FSlateWindowElementList;
class SDragScrollPanel;


/** SDragScrollBox can scroll through an arbitrary number of widgets. */
class VRDEBUGPLUGIN_API SDragScrollBox : public SCompoundWidget
{
public:
	/** A Slot that provides layout options for the contents of a scrollable box. */
	using FSlot = FBasicLayoutWidgetSlot;

	SLATE_BEGIN_ARGS(SDragScrollBox)
		: _Style( &FAppStyle::Get().GetWidgetStyle<FScrollBoxStyle>("ScrollBox") )
		, _ScrollBarStyle( &FAppStyle::Get().GetWidgetStyle<FScrollBarStyle>("ScrollBar") )
		, _ExternalScrollbar()
		, _Orientation(Orient_Vertical)
		, _ScrollBarVisibility(EVisibility::Visible)
		, _ScrollBarAlwaysVisible(false)
		, _ScrollBarDragFocusCause(EFocusCause::Mouse)
		, _ScrollBarThickness(FVector2D(_Style->BarThickness, _Style->BarThickness))
		, _ScrollBarPadding(2.0f)
		, _AllowOverscroll(EAllowOverscroll::Yes)
		, _BackPadScrolling(false)
		, _FrontPadScrolling(false)
		, _AnimateWheelScrolling(false)
		, _WheelScrollMultiplier(1.f)
		, _NavigationDestination(EDescendantScrollDestination::IntoView)
		, _NavigationScrollPadding(0.0f)
		, _ScrollWhenFocusChanges(EScrollWhenFocusChanges::NoScroll)
		, _OnUserScrolled()
		, _ConsumeMouseWheel(EConsumeMouseWheel::WhenScrollingPossible)
		{
			_Clipping = EWidgetClipping::ClipToBounds;
		}
		
		SLATE_SLOT_ARGUMENT( FSlot, Slots )

		/** Style used to draw this scrollbox */
		SLATE_STYLE_ARGUMENT( FScrollBoxStyle, Style )

		/** Style used to draw this scrollbox's scrollbar */
		SLATE_STYLE_ARGUMENT( FScrollBarStyle, ScrollBarStyle )

		/** Custom scroll bar */
		SLATE_ARGUMENT( TSharedPtr<SScrollBar>, ExternalScrollbar )

		/** The direction that children will be stacked, and also the direction the box will scroll. */
		SLATE_ARGUMENT( EOrientation, Orientation )

		SLATE_ARGUMENT( EVisibility, ScrollBarVisibility )

		SLATE_ARGUMENT( bool, ScrollBarAlwaysVisible )

		SLATE_ARGUMENT( EFocusCause, ScrollBarDragFocusCause )

		SLATE_ARGUMENT( FVector2D, ScrollBarThickness )

		SLATE_ARGUMENT( FMargin, ScrollBarPadding )

		SLATE_ARGUMENT(EAllowOverscroll, AllowOverscroll);

		SLATE_ARGUMENT(bool, BackPadScrolling);

		SLATE_ARGUMENT(bool, FrontPadScrolling);

		SLATE_ARGUMENT(bool, AnimateWheelScrolling);

		SLATE_ARGUMENT(float, WheelScrollMultiplier);

		SLATE_ARGUMENT(EDescendantScrollDestination, NavigationDestination);

		/**
		 * The amount of padding to ensure exists between the item being navigated to, at the edge of the
		 * scrollbox.  Use this if you want to ensure there's a preview of the next item the user could scroll to.
		 */
		SLATE_ARGUMENT(float, NavigationScrollPadding);

		SLATE_ARGUMENT(EScrollWhenFocusChanges, ScrollWhenFocusChanges);

		/** Called when the button is clicked */
		SLATE_EVENT(FOnUserScrolled, OnUserScrolled)

		SLATE_ARGUMENT(EConsumeMouseWheel, ConsumeMouseWheel);

	SLATE_END_ARGS()

	SDragScrollBox();

	/** @return a new slot. Slots contain children for SDragScrollBox */
	static FSlot::FSlotArguments Slot();

	void Construct( const FArguments& InArgs );

	using FScopedWidgetSlotArguments = TPanelChildren<FSlot>::FScopedWidgetSlotArguments;
	/** Adds a slot to SDragScrollBox */
	FScopedWidgetSlotArguments AddSlot();

	/** Removes a slot at the specified location */
	void RemoveSlot( const TSharedRef<SWidget>& WidgetToRemove );

	/** Removes all children from the box */
	void ClearChildren();

	/** @return Returns true if the user is currently interactively scrolling the view by holding
		        the right mouse button and dragging. */
	bool IsRightClickScrolling() const;

	EAllowOverscroll GetAllowOverscroll() const;

	void SetAllowOverscroll( EAllowOverscroll NewAllowOverscroll );

	void SetAnimateWheelScrolling(bool bInAnimateWheelScrolling);

	void SetWheelScrollMultiplier(float NewWheelScrollMultiplier);
	
	void SetScrollWhenFocusChanges(EScrollWhenFocusChanges NewScrollWhenFocusChanges);

	float GetScrollOffset() const;

	float GetViewFraction() const;

	float GetViewOffsetFraction() const;

	/** Gets the scroll offset of the bottom of the ScrollBox in Slate Units. */
	float GetScrollOffsetOfEnd() const;

	void SetScrollOffset( float NewScrollOffset );

	void ScrollToStart();

	void ScrollToEnd();

	void EndInertialScrolling();

	/** 
	 * Attempt to scroll a widget into view, will safely handle non-descendant widgets 
	 *
	 * @param WidgetToFind The widget whose geometry we wish to discover.
	 * @param InAnimateScroll	Whether or not to animate the scroll
	 * @param InDestination		Where we want the child widget to stop.
	 */
	void ScrollDescendantIntoView(const TSharedPtr<SWidget>& WidgetToFind, bool InAnimateScroll = true, EDescendantScrollDestination InDestination = EDescendantScrollDestination::IntoView, float Padding = 0);

	/** Get the current orientation of the scrollbox. */
	EOrientation GetOrientation();

	void SetNavigationDestination(const EDescendantScrollDestination NewNavigationDestination);

	void SetConsumeMouseWheel(EConsumeMouseWheel NewConsumeMouseWheel);

	/** Sets the current orientation of the scrollbox and updates the layout */
	void SetOrientation(EOrientation InOrientation);

	void SetScrollBarVisibility(EVisibility InVisibility);

	void SetScrollBarAlwaysVisible(bool InAlwaysVisible);
	
	void SetScrollBarTrackAlwaysVisible(bool InAlwaysVisible);

	void SetScrollBarThickness(FVector2D InThickness);

	void SetScrollBarPadding(const FMargin& InPadding);

	void SetScrollBarRightClickDragAllowed(bool bIsAllowed);
public:

	// SWidget interface
	virtual void Tick( const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime ) override;
	virtual bool ComputeVolatility() const override;
	virtual FReply OnPreviewMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonDown( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) override;
	virtual FReply OnMouseButtonUp( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) override;
	virtual FReply OnMouseMove( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) override;
	virtual void OnMouseEnter( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) override;
	virtual void OnMouseLeave( const FPointerEvent& MouseEvent ) override;
	virtual FReply OnMouseWheel( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) override;
	virtual FCursorReply OnCursorQuery( const FGeometry& MyGeometry, const FPointerEvent& CursorEvent ) const override;
	virtual int32 OnPaint( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const override;
	virtual FReply OnTouchEnded(const FGeometry& MyGeometry, const FPointerEvent& InTouchEvent) override;
	virtual void OnMouseCaptureLost(const FCaptureLostEvent& CaptureLostEvent) override;
	virtual FNavigationReply OnNavigation(const FGeometry& MyGeometry, const FNavigationEvent& InNavigationEvent) override;
	virtual void OnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath, const FFocusEvent& InFocusEvent) override;
	// End of SWidget interface

protected:
	void OnClippingChanged();

private:

	/** Builds a default Scrollbar */
	TSharedPtr<SScrollBar> ConstructScrollBar();

	/** Constructs internal layout widgets for scrolling vertically using the existing ScrollPanel and ScrollBar. */
	void ConstructVerticalLayout();

	/** Constructs internal layout widgets for scrolling horizontally using the existing ScrollPanel and ScrollBar. */
	void ConstructHorizontalLayout();

	/** Gets the component of a vector in the direction of scrolling based on the Orientation property. */
	FORCEINLINE float GetScrollComponentFromVector(FVector2D Vector) const
	{
		return Orientation == Orient_Vertical ? Vector.Y : Vector.X;
	}

	/** Sets the component of a vector in the direction of scrolling based on the Orientation property. */
	inline void SetScrollComponentOnVector(FVector2D& InVector, float Value) const
	{
		if (Orientation == Orient_Vertical)
		{
			InVector.Y = Value;
		}
		else
		{
			InVector.X = Value;
		}
	}

	/** Scroll offset that the user asked for. We will clamp it before actually scrolling there. */
	float DesiredScrollOffset;

	/**
	 * Scroll the view by ScrollAmount given its currently AllottedGeometry.
	 *
	 * @param AllottedGeometry  The geometry allotted for this SDragScrollBox by the parent
	 * @param ScrollAmount      
	 * @param InAnimateScroll	Whether or not to animate the scroll
	 * @return Whether or not the scroll was fully handled
	 */
	bool ScrollBy(const FGeometry& AllottedGeometry, float LocalScrollAmount, EAllowOverscroll Overscroll, bool InAnimateScroll);

	/** Invoked when the user scroll via the scrollbar */
	void ScrollBar_OnUserScrolled( float InScrollOffsetFraction );

	/** Does the user need a hint that they can scroll to the start of the list? */
	FSlateColor GetStartShadowOpacity() const;
	
	/** Does the user need a hint that they can scroll to the end of the list? */
	FSlateColor GetEndShadowOpacity() const;

	/** Active timer to update inertial scrolling as needed */
	EActiveTimerReturnType UpdateInertialScroll(double InCurrentTime, float InDeltaTime);

	/** Check whether the current state of the table warrants inertial scroll by the specified amount */
	bool CanUseInertialScroll(float ScrollAmount) const;

	void BeginInertialScrolling();

	/** Padding to the scrollbox */
	FMargin ScrollBarSlotPadding;

	union
	{
		// vertical scroll bar is stored in horizontal box and vice versa
		SHorizontalBox::FSlot* VerticalScrollBarSlot; // valid when Orientation == Orient_Vertical
		SVerticalBox::FSlot* HorizontalScrollBarSlot; // valid when Orientation == Orient_Horizontal
	};

protected:
	/** Scrolls or begins scrolling a widget into view, only valid to call when we have layout geometry. */
	bool InternalScrollDescendantIntoView(const FGeometry& MyGeometry, const TSharedPtr<SWidget>& WidgetToFind, bool InAnimateScroll = true, EDescendantScrollDestination InDestination = EDescendantScrollDestination::IntoView, float Padding = 0);

	/** returns widget that can receive keyboard focus or nullprt **/
	TSharedPtr<SWidget> GetKeyboardFocusableWidget(TSharedPtr<SWidget> InWidget);

	/** The panel which stacks the child slots */
	TSharedPtr<class SDragScrollPanel> ScrollPanel;

	/** The scrollbar which controls scrolling for the scrollbox. */
	TSharedPtr<SScrollBar> ScrollBar;

	/** The amount we have scrolled this tick cycle */
	float TickScrollDelta;

	/** Did the user start an interaction in this list? */
	TOptional<int32> bFingerOwningTouchInteraction;

	/** How much we scrolled while the rmb has been held */
	float AmountScrolledWhileRightMouseDown;

	/** The current deviation we've accumulated on scrol, once it passes the trigger amount, we're going to begin scrolling. */
	float PendingScrollTriggerAmount;

	/** Helper object to manage inertial scrolling */
	FInertialScrollManager InertialScrollManager;

	/** The overscroll state management structure. */
	FOverscroll Overscroll;

	/** Whether to permit overscroll on this scroll box */
	EAllowOverscroll AllowOverscroll;

#if WITH_EDITORONLY_DATA
	/** Padding to the scrollbox */
	UE_DEPRECATED(5.0, "ScrollBarPadding is deprecated, Use SetScrollBarPadding")
	FMargin ScrollBarPadding;
#endif

	/** Whether to back pad this scroll box, allowing user to scroll backward until child contents are no longer visible */
	bool BackPadScrolling;

	/** Whether to front pad this scroll box, allowing user to scroll forward until child contents are no longer visible */
	bool FrontPadScrolling;

	/**
	 * The amount of padding to ensure exists between the item being navigated to, at the edge of the
	 * scrollbox.  Use this if you want to ensure there's a preview of the next item the user could scroll to.
	 */
	float NavigationScrollPadding;

	/** Sets where to scroll a widget to when using explicit navigation or if ScrollWhenFocusChanges is enabled */
	EDescendantScrollDestination NavigationDestination;

	/** Scroll behavior when user focus is given to a child widget */
	EScrollWhenFocusChanges ScrollWhenFocusChanges;

	/**	The current position of the software cursor */
	FVector2D SoftwareCursorPosition;

	/** Fired when the user scrolls the scrollbox */
	FOnUserScrolled OnUserScrolled;

	/** The scrolling and stacking orientation. */
	EOrientation Orientation;

	/** Style resource for the scrollbox */
	const FScrollBoxStyle* Style;

	/** Style resource for the scrollbar */
	const FScrollBarStyle* ScrollBarStyle;

	/** How we should handle scrolling with the mouse wheel */
	EConsumeMouseWheel ConsumeMouseWheel;

	/** Cached geometry for use with the active timer */
	FGeometry CachedGeometry;

	/** Scroll into view request. */
	TFunction<void(FGeometry)> ScrollIntoViewRequest;

	TSharedPtr<FActiveTimerHandle> UpdateInertialScrollHandle;

	double LastScrollTime;

	/** Multiplier applied to each click of the scroll wheel (applied alongside the global scroll amount) */
	float WheelScrollMultiplier = 1.f;

	/** Whether to animate wheel scrolling */
	bool bAnimateWheelScrolling : 1;

	/**	Whether the software cursor should be drawn in the viewport */
	bool bShowSoftwareCursor : 1;

	/** Whether or not the user supplied an external scrollbar to control scrolling. */
	bool bScrollBarIsExternal : 1;

	/** Are we actively scrolling right now */
	bool bIsScrolling : 1;

	/** Should the current scrolling be animated or immediately jump to the desired scroll offer */
	bool bAnimateScroll : 1;

	/** If true, will scroll to the end next Tick */
	bool bScrollToEnd : 1;

	/** Whether the active timer to update the inertial scroll is registered */
	bool bIsScrollingActiveTimerRegistered : 1;

	bool bAllowsRightClickDragScrolling : 1;
	
	bool bTouchPanningCapture : 1;
};

class SDragScrollPanel : public SPanel
{
public:

	SLATE_BEGIN_ARGS(SDragScrollPanel)
	{
		_Visibility = EVisibility::SelfHitTestInvisible;
	}

	SLATE_ARGUMENT(EOrientation, Orientation)
	SLATE_ARGUMENT(bool, BackPadScrolling)
	SLATE_ARGUMENT(bool, FrontPadScrolling)

		SLATE_END_ARGS()

		SDragScrollPanel()
		: Children(this)
	{
	}

	UE_DEPRECATED(5.0, "Direct construction of FSlot is deprecated")
	void Construct(const FArguments& InArgs, const TArray<SDragScrollBox::FSlot*>& InSlots);

	void Construct(const FArguments& InArgs, TArray<SDragScrollBox::FSlot::FSlotArguments> InSlots);

public:

	EOrientation GetOrientation()
	{
		return Orientation;
	}

	void SetOrientation(EOrientation InOrientation)
	{
		Orientation = InOrientation;
	}

	virtual void OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const override;

	virtual FChildren* GetChildren() override
	{
		return &Children;
	}

	float PhysicalOffset;
	TPanelChildren<SDragScrollBox::FSlot> Children;

protected:
	// Begin SWidget overrides.
	virtual FVector2D ComputeDesiredSize(float) const override;
	// End SWidget overrides.

private:

	float ArrangeChildVerticalAndReturnOffset(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren, const SDragScrollBox::FSlot& ThisSlot, float CurChildOffset) const;
	float ArrangeChildHorizontalAndReturnOffset(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren, const SDragScrollBox::FSlot& ThisSlot, float CurChildOffset) const;

private:

	EOrientation Orientation;
	bool BackPadScrolling;
	bool FrontPadScrolling;
};
#else
#include "Runtime/Slate/Public/Widgets/Layout/SScrollBox.h"

class FPaintArgs;
class FSlateWindowElementList;
class SScrollPanel;

class VRDEBUGPLUGIN_API SDragScrollBox : public SCompoundWidget
{
public:
	/** A Slot that provides layout options for the contents of a scrollable box. */
	class VRDEBUGPLUGIN_API FSlot : public TSlotBase<FSlot>, public TSupportsContentPaddingMixin<FSlot>
	{
	public:
		FSlot()
		: TSlotBase<FSlot>()
		// Set both vertical and horizontal alignment to fill by default.  During layout, the
		// alignment direction parallel to the scroll direction is assumed to be top, or left
		// since that is how the items are stacked.
		, HAlignment(HAlign_Fill)
		, VAlignment(VAlign_Fill)
		{
		}

		FSlot& HAlign(EHorizontalAlignment InHAlignment)
		{
			HAlignment = InHAlignment;
			return *this;
		}

		FSlot& VAlign(EVerticalAlignment InVAlignment)
		{
			VAlignment = InVAlignment;
			return *this;
		}

		EHorizontalAlignment HAlignment;
		EVerticalAlignment VAlignment;
	};

	SLATE_BEGIN_ARGS(SDragScrollBox)
		: _Style(&FCoreStyle::Get().GetWidgetStyle<FScrollBoxStyle>("ScrollBox"))
		, _ScrollBarStyle(&FCoreStyle::Get().GetWidgetStyle<FScrollBarStyle>("ScrollBar"))
		, _ExternalScrollbar()
		, _Orientation(Orient_Vertical)
		, _ScrollBarVisibility(EVisibility::Visible)
		, _ScrollBarAlwaysVisible(false)
		, _ScrollBarDragFocusCause(EFocusCause::Mouse)
		, _ScrollBarThickness(FVector2D(5, 5))
		, _AllowOverscroll(EAllowOverscroll::Yes)
		, _NavigationDestination(EDescendantScrollDestination::IntoView)
		, _NavigationScrollPadding(0.0f)
		, _OnUserScrolled()
		, _ConsumeMouseWheel(EConsumeMouseWheel::WhenScrollingPossible)
	{
		_Clipping = EWidgetClipping::ClipToBounds;
	}

	SLATE_SUPPORTS_SLOT(FSlot)

	/** Style used to draw this scrollbox */
	SLATE_STYLE_ARGUMENT(FScrollBoxStyle, Style)

	/** Style used to draw this scrollbox's scrollbar */
	SLATE_STYLE_ARGUMENT(FScrollBarStyle, ScrollBarStyle)

	/** Custom scroll bar */
	SLATE_ARGUMENT(TSharedPtr<SScrollBar>, ExternalScrollbar)

	/** The direction that children will be stacked, and also the direction the box will scroll. */
	SLATE_ARGUMENT(EOrientation, Orientation)

	SLATE_ARGUMENT(EVisibility, ScrollBarVisibility)

	SLATE_ARGUMENT(bool, ScrollBarAlwaysVisible)

	SLATE_ARGUMENT(EFocusCause, ScrollBarDragFocusCause)

	SLATE_ARGUMENT(FVector2D, ScrollBarThickness)

	SLATE_ARGUMENT(EAllowOverscroll, AllowOverscroll);

	SLATE_ARGUMENT(EDescendantScrollDestination, NavigationDestination);

	/**
	 * The amount of padding to ensure exists between the item being navigated to, at the edge of the
	 * scrollbox.  Use this if you want to ensure there's a preview of the next item the user could scroll to.
	 */
	SLATE_ARGUMENT(float, NavigationScrollPadding);

	/** Called when the button is clicked */
	SLATE_EVENT(FOnUserScrolled, OnUserScrolled)

	SLATE_ARGUMENT(EConsumeMouseWheel, ConsumeMouseWheel);

	SLATE_END_ARGS()

	SDragScrollBox();

	/** @return a new slot. Slots contain children for SScrollBox */
	static FSlot& Slot();

	void Construct(const FArguments& InArgs);

	/** Adds a slot to SScrollBox */
	SDragScrollBox::FSlot& AddSlot();

	/** Removes a slot at the specified location */
	void RemoveSlot(const TSharedRef<SWidget>& WidgetToRemove);

	/** Removes all children from the box */
	void ClearChildren();

	/** @return Returns true if the user is currently interactively scrolling the view by holding
				the right mouse button and dragging. */
	bool IsRightClickScrolling() const;

	EAllowOverscroll GetAllowOverscroll() const;

	void SetAllowOverscroll(EAllowOverscroll NewAllowOverscroll);

	float GetScrollOffset() const;

	float GetViewFraction() const;

	float GetViewOffsetFraction() const;

	void SetScrollOffset(float NewScrollOffset);

	void ScrollToStart();

	void ScrollToEnd();

	/**
	 * Attempt to scroll a widget into view, will safely handle non-descendant widgets
	 *
	 * @param WidgetToFind The widget whose geometry we wish to discover.
	 * @param InAnimateScroll	Whether or not to animate the scroll
	 * @param InDestination		Where we want the child widget to stop.
	 */
	void ScrollDescendantIntoView(const TSharedPtr<SWidget>& WidgetToFind, bool InAnimateScroll = true, EDescendantScrollDestination InDestination = EDescendantScrollDestination::IntoView, float Padding = 0);

	/** Get the current orientation of the scrollbox. */
	EOrientation GetOrientation();

	/** Sets the current orientation of the scrollbox and updates the layout */
	void SetOrientation(EOrientation InOrientation);

	void SetScrollBarVisibility(EVisibility InVisibility);

	void SetScrollBarAlwaysVisible(bool InAlwaysVisible);

	void SetScrollBarThickness(FVector2D InThickness);

	void SetScrollBarRightClickDragAllowed(bool bIsAllowed);
public:

	// SWidget interface
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
	virtual bool ComputeVolatility() const override;
	virtual FReply OnPreviewMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual void OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual void OnMouseLeave(const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FCursorReply OnCursorQuery(const FGeometry& MyGeometry, const FPointerEvent& CursorEvent) const override;
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	virtual FReply OnTouchEnded(const FGeometry& MyGeometry, const FPointerEvent& InTouchEvent) override;
	virtual void OnMouseCaptureLost(const FCaptureLostEvent& CaptureLostEvent) override;
	virtual FNavigationReply OnNavigation(const FGeometry& MyGeometry, const FNavigationEvent& InNavigationEvent) override;
	// End of SWidget interface

protected:
	void OnClippingChanged();

private:

	/** Builds a default Scrollbar */
	TSharedPtr<SScrollBar> ConstructScrollBar();

	/** Constructs internal layout widgets for scrolling vertically using the existing ScrollPanel and ScrollBar. */
	void ConstructVerticalLayout();

	/** Constructs internal layout widgets for scrolling horizontally using the existing ScrollPanel and ScrollBar. */
	void ConstructHorizontalLayout();

	/** Gets the component of a vector in the direction of scrolling based on the Orientation property. */
	FORCEINLINE float GetScrollComponentFromVector(FVector2D Vector) const
	{
		return Orientation == Orient_Vertical ? Vector.Y : Vector.X;
	}

	/** Sets the component of a vector in the direction of scrolling based on the Orientation property. */
	inline void SetScrollComponentOnVector(FVector2D& InVector, float Value) const
	{
		if (Orientation == Orient_Vertical)
		{
			InVector.Y = Value;
		}
		else
		{
			InVector.X = Value;
		}
	}

	/** Scroll offset that the user asked for. We will clamp it before actually scrolling there. */
	float DesiredScrollOffset;

	/** Scrolls or begins scrolling a widget into view, only valid to call when we have layout geometry. */
	bool InternalScrollDescendantIntoView(const FGeometry& MyGeometry, const TSharedPtr<SWidget>& WidgetToFind, bool InAnimateScroll = true, EDescendantScrollDestination InDestination = EDescendantScrollDestination::IntoView, float Padding = 0);

	/**
	 * Scroll the view by ScrollAmount given its currently AllottedGeometry.
	 *
	 * @param AllottedGeometry  The geometry allotted for this SScrollBox by the parent
	 * @param ScrollAmount
	 * @param InAnimateScroll	Whether or not to animate the scroll
	 * @return Whether or not the scroll was fully handled
	 */
	bool ScrollBy(const FGeometry& AllottedGeometry, float LocalScrollAmount, EAllowOverscroll Overscroll, bool InAnimateScroll);

	/** Invoked when the user scroll via the scrollbar */
	void ScrollBar_OnUserScrolled(float InScrollOffsetFraction);

	/** Does the user need a hint that they can scroll to the start of the list? */
	FSlateColor GetStartShadowOpacity() const;

	/** Does the user need a hint that they can scroll to the end of the list? */
	FSlateColor GetEndShadowOpacity() const;

	/** Active timer to update inertial scrolling as needed */
	EActiveTimerReturnType UpdateInertialScroll(double InCurrentTime, float InDeltaTime);

	/** Check whether the current state of the table warrants inertial scroll by the specified amount */
	bool CanUseInertialScroll(float ScrollAmount) const;

	void BeginInertialScrolling();

	void EndInertialScrolling();

private:

	/** The panel which stacks the child slots */
	TSharedPtr<class SDragScrollPanel> ScrollPanel;

	/** The scrollbar which controls scrolling for the scrollbox. */
	TSharedPtr<SScrollBar> ScrollBar;

	/** The amount we have scrolled this tick cycle */
	float TickScrollDelta;

	/** Did the user start an interaction in this list? */
	TOptional<int32> bFingerOwningTouchInteraction;

	/** How much we scrolled while the rmb has been held */
	float AmountScrolledWhileRightMouseDown;

	/** The current deviation we've accumulated on scrol, once it passes the trigger amount, we're going to begin scrolling. */
	float PendingScrollTriggerAmount;

	/** Helper object to manage inertial scrolling */
	FInertialScrollManager InertialScrollManager;

	/** The overscroll state management structure. */
	FOverscroll Overscroll;

	/** Whether to permit overscroll on this scroll box */
	EAllowOverscroll AllowOverscroll;

	/**
	 * The amount of padding to ensure exists between the item being navigated to, at the edge of the
	 * scrollbox.  Use this if you want to ensure there's a preview of the next item the user could scroll to.
	 */
	float NavigationScrollPadding;

	/**
	 *
	 */
	EDescendantScrollDestination NavigationDestination;

	/**	The current position of the software cursor */
	FVector2D SoftwareCursorPosition;

	/** Fired when the user scrolls the scrollbox */
	FOnUserScrolled OnUserScrolled;

	/** The scrolling and stacking orientation. */
	EOrientation Orientation;

	/** Style resource for the scrollbox */
	const FScrollBoxStyle* Style;

	/** Style resource for the scrollbar */
	const FScrollBarStyle* ScrollBarStyle;

	/** How we should handle scrolling with the mouse wheel */
	EConsumeMouseWheel ConsumeMouseWheel;

	/** Cached geometry for use with the active timer */
	FGeometry CachedGeometry;

	/** Scroll into view request. */
	TFunction<void(FGeometry)> ScrollIntoViewRequest;

	TSharedPtr<FActiveTimerHandle> UpdateInertialScrollHandle;

	double LastScrollTime;

	/**	Whether the software cursor should be drawn in the viewport */
	bool bShowSoftwareCursor : 1;

	/** Whether or not the user supplied an external scrollbar to control scrolling. */
	bool bScrollBarIsExternal : 1;

	/** Are we actively scrolling right now */
	bool bIsScrolling : 1;

	/** Should the current scrolling be animated or immediately jump to the desired scroll offer */
	bool bAnimateScroll : 1;

	/** If true, will scroll to the end next Tick */
	bool bScrollToEnd : 1;

	/** Whether the active timer to update the inertial scroll is registered */
	bool bIsScrollingActiveTimerRegistered : 1;

	bool bAllowsRightClickDragScrolling : 1;

	bool bTouchPanningCapture : 1;
	
};
#endif

