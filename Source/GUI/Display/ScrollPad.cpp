/*
  ==============================================================================

    ScrollPad.cpp
    Created: 5 Oct 2022 7:30:04pm
    Author:  Joe

  ==============================================================================
*/

#include "ScrollPad.h"

using namespace juce;

// Scroll Pad
ScrollPad::ScrollPad()
{
}

ScrollPad::~ScrollPad()
{
}

void ScrollPad::resized()
{
}

// Takes In Saved Points For Initialization
void ScrollPad::initializePoints(float newP1, float newP2)
{
	// Prevent Crossing Points via Parameters List
	if (point2 - point1 >= minWidth)
	{
		point1 = newP1;
		point2 = newP2;

		currentCenter = float(point1 + point2) / 2.f;
		currentWidth = point2 - point1;

		calculateZoomFactor();
	}
	else
		makeDefaultPoints();
}

// Change Points & Calculate P/Z Upon Movement
void ScrollPad::updatePoints(int x1, int y1)
{
	auto bounds = getLocalBounds();

	int edgeTolerance = bounds.getX() + bounds.getCentreX() - maxWidth / 2.f;

	// Mouse is clicked on or beyond left thumb
	if (shouldUpdatePoint1)
	{
		point1 = x1;

		if (point1 < bounds.getCentreX() - (float)maxWidth / 2.f)
			point1 = bounds.getCentreX() - (float)maxWidth / 2.f;

		// Halt At Minimum Width
		if (point2 - point1 < minWidth)
			point1 = point2 - minWidth;

		currentCenter = (float)(point1 + point2) / 2.f;
		currentWidth = point2 - point1;
	}

	// Mouse is clicked on or beyond right thumb
	if (shouldUpdatePoint2)
	{
		point2 = x1;

		if (point2 > bounds.getCentreX() + (float)maxWidth / 2.f)
			point2 = bounds.getCentreX() + (float)maxWidth / 2.f;

		// Halt At Minimum Width
		if (point2 - point1 < minWidth)
			point2 = point1 + minWidth;

		currentCenter = (float)(point1 + point2) / 2.f;
		currentWidth = point2 - point1;
	}

	// Mouse is clicked in between thumbs
	if (shouldCheckPanOrZoom)
	{
		// Distance moved since last 'Starting Point'
		float xAbs = (x1 - x0); // Delta X after breaking distance threshold
		float yAbs = (y1 - y0);	// Delta Y after breaking distance threshold
		float hAbs = sqrt(pow(xAbs, 2) + pow(yAbs, 2)); // Absolute Distance

		float ang = 0.f;			// Angle of mouse-move every 3 pixels
		bool shouldZoom = false;	// Flagged if vertical mouse movement
		bool shouldPan = false;		// Flagged if horizontal mouse movement
		float distThreshold = 1;	// Distance before resetting 'Current Position'

		// Once distance-threshold is met, calculate mouse-direction and determine Pan or Zoom
		if (abs(hAbs) > distThreshold)
		{
			ang = abs(juce::radiansToDegrees(asin(yAbs / hAbs)));

			if (ang < 60)
				shouldPan = true;	// Boundary Checking On Pan Engaged
			else
				shouldZoom = true;	// Boundary Checking On Zoom Engaged
		}

		// Apply Zoom... Larger Upward Motion = Larger Zoom Increment

		if (shouldZoom)
		{
			zoomScale = juce::jmap(ang, 0.f, 90.f, 0.f, zoomIncrement);

			// When Zooming In... Shrink 'Width' until Min
			if (yAbs < 0.f)
			{
				currentWidth -= zoomScale;

				if (currentWidth < minWidth)
					currentWidth = minWidth;
			}

			// When Zooming Out... Expand 'Width' until Max
			if (yAbs > 0.f)
			{
				currentWidth += zoomScale;

				if (currentWidth > maxWidth)
					currentWidth = maxWidth;
			}
		}

		int c0 = (p01 + p02) / 2.f; // Actual Center of Points at time of Downclick
		int offset = xDown - c0;	// Distance between Mouse Down and Points-Center

		currentCenter = x1 - offset;		// Center of points as a function of mouse position

		//// Disable Panning Function
		//currentCenter = bounds.getCentreX();

		// Prevent Panning Beyond Left Edge
		if (currentCenter < bounds.getX() + edgeTolerance + currentWidth / 2.f)
			currentCenter = bounds.getX() + edgeTolerance + currentWidth / 2.f;

		// Prevent Panning Beyond Right Edge
		if (currentCenter > bounds.getRight() - edgeTolerance - currentWidth / 2.f)
			currentCenter = bounds.getRight() - edgeTolerance - currentWidth / 2.f;

		// Update Points
		point1 = currentCenter - currentWidth / 2.f;
		point2 = currentCenter + currentWidth / 2.f;

		// 'Current' Mouse Position become 'Old' position
		x0 = x1;
		y0 = y1;
	}

	// Prevent Point1 from Protruding Left-Side
	if (point1 < bounds.getX() + edgeTolerance)
		point1 = bounds.getX() + edgeTolerance;

	// Prevent Point2 from Protruding Right-Side
	if (point2 > bounds.getRight() - edgeTolerance)
		point2 = bounds.getRight() - edgeTolerance;

	calculateZoomFactor();
}

void ScrollPad::paint(juce::Graphics& g)
{
	// Establish Bounds
	auto bounds = getLocalBounds().toFloat();

	// Draw Slider Track
	juce::Path track;
	track.startNewSubPath(bounds.getCentreX() - maxWidth / 2.f, bounds.getCentreY());
	track.lineTo(bounds.getCentreX() + maxWidth / 2.f, bounds.getCentreY());
	g.setColour(juce::Colours::darkgrey.withMultipliedBrightness(0.4f));
	g.strokePath(track, juce::PathStrokeType(6.f, juce::PathStrokeType::JointStyle::beveled,
		juce::PathStrokeType::EndCapStyle::rounded));

	// Draw Point-To-Point Range
	juce::Path range;
	range.startNewSubPath(point1, bounds.getCentreY());
	range.lineTo(point2, bounds.getCentreY());
	g.setColour(juce::Colours::purple);
	g.strokePath(range, juce::PathStrokeType(6.f, juce::PathStrokeType::JointStyle::beveled,
		juce::PathStrokeType::EndCapStyle::rounded));

	auto size = 10;
	g.setColour(juce::Colours::lightgrey);
}

// Reset Pan & Zoom to Default
void ScrollPad::makeDefaultPoints()
{
	auto bounds = getLocalBounds();

	currentCenter = bounds.getCentreX();
	point1 = currentCenter - defaultWidth / 2;
	point2 = currentCenter + defaultWidth / 2;

	calculateZoomFactor();
}

// Record Mouse-Down Positions
void ScrollPad::mouseDown(const juce::MouseEvent& event)
{
	// Initial Mouse Position
	xDown = event.getMouseDownX();
	yDown = event.getMouseDownY();

	// Historical Mouse Position
	x0 = xDown;
	y0 = yDown;

	// Historical Point1/2 Positions
	p01 = point1;
	p02 = point2;

	// Reset Flags
	shouldUpdatePoint1 = false;
	shouldUpdatePoint2 = false;
	shouldCheckPanOrZoom = false;

	int clickTolerance = 5;

	// Mouse is clicked on or beyond Point1
	if (xDown < point1 + clickTolerance)
		shouldUpdatePoint1 = true;

	// Mouse is clicked in between points
	if (xDown >= point1 + clickTolerance && xDown <= point2 - clickTolerance)
		shouldCheckPanOrZoom = true;

	// Mouse is clicked on or beyond Point2
	if (xDown > point2 - clickTolerance)
		shouldUpdatePoint2 = true;
}

// Mouse Drag Callback
void ScrollPad::mouseDrag(const juce::MouseEvent& event)
{
	auto bounds = getLocalBounds();

	// Immediate Mouse Position
	x1 = event.getPosition().getX();
	y1 = event.getPosition().getY();

	updatePoints(x1, y1);
}

// Mouse Double-Click Callback
void ScrollPad::mouseDoubleClick(const juce::MouseEvent& event)
{
	makeDefaultPoints();
}

// Take Width and Derive Zoom Factor
void ScrollPad::calculateZoomFactor()
{
	currentWidth = point2 - point1;
	currentZoomFactor = juce::jmap((float)currentWidth, (float)minWidth, (float)maxWidth, minZoomFactor, maxZoomFactor);
}

// Returns Current Center Position
float ScrollPad::getCenter()
{
	// Center should represent a percentage of the total waveform.
	auto bounds = getLocalBounds().toFloat();
	auto centerScaled = juce::jmap((float)currentCenter,
		bounds.getCentreX() - maxWidth / 2.f,
		bounds.getCentreX() + maxWidth / 2.f,
		+0.5f, -0.5f);

	return centerScaled;
}

// Returns Current Zoom Value
float ScrollPad::getZoom()
{
	return currentZoomFactor;
}