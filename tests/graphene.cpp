//	Copyright (C) 2014, 2018 Vaptistis Anogeianakis <nomad@cornercase.gr>
/*
 *	This file is part of LostArt.
 *
 *	LostArt is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	LostArt is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with LostArt.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "graphene.hpp"
using namespace graphene;
using namespace Frames;
using namespace Frames::Stateful;
using namespace Frames::Behavioural;
using namespace Frames::EventHandling;
using namespace Frames::Renderable;
using namespace DSEL;

#include <ratio>

#define BOOST_TEST_MODULE Graphene
#include <boost/test/included/unit_test.hpp>

// This test suite is intended to test properties related to how graphene classes interact with each other.
// Declare realistic frame stacks to verify they compile and behave as expected.

template<typename RectangleType, typename BorderSize, typename Margin, typename TextType = std::string>
using Button = FrameStack<
	RectangleType,
	Frame<UniformlyBordered, typename RectangleType::coordinate_type>,
	Frame<Pressable>,
	Frame<Highlightable>,
	Frame<Textual, TextType>,
	Frame<SizedText, FunctionObjects::GlutStrokeFontEngine>,
	Frame<BoxedAdaptableSizeText, Margin>,
	Frame<TwoStagePressable>,
	Condition<FunctionObjects::Pressed,
		Sequence<
			Frame<Colorblind::FilledRectangle>,
			FrameStack<
				Frame<Colorblind::BoxedText, FunctionObjects::Textual>,
				Frame<Colorblind::InversedColor>>>,
		Condition<FunctionObjects::Highlighted,
			Sequence<
				Frame<Colorblind::BorderedRectangle, BorderSize>,
				Frame<Colorblind::BoxedText, FunctionObjects::Textual>>,
			Sequence<
				Frame<Colorblind::BorderedRectangle, std::ratio<0>>,
				Frame<Colorblind::BoxedText, FunctionObjects::Textual>>
		>
	>
>;

template<typename CoordinateType>
using IShapePart = FrameStack<
	Interface::Empty,
	Frame<Interface::Destructible>,
	Frame<Interface::Movable, CoordinateType>,
	Frame<Interface::Containing>,
	Frame<Interface::Renderable>
>;

/** Const instances should be constant and non-const instances should be non constant
 */
template<typename CoordinateType, typename horizontallyMovable, typename verticallyMovable, bool leftRef, bool bottomRef, bool rightRef, bool topRef>
using ControlPart = FrameStack<
	IShapePart<CoordinateType>,
	Frame<Interface::Rectangular>,
	Frame<Movable>,
	Frame<HVMovable, horizontallyMovable, verticallyMovable>,
	Frame<Colorblind::FilledRectangle>,
	Frame<Stippled>,
	Frame<Colorblind::InversedColor>,
	Frame<Rectangular, geometry::RefRectangle<CoordinateType, leftRef, bottomRef, rightRef, topRef>>
>;

template<typename CoordinateType, typename TextType = std::string>
using IControl = FrameStack<
	Interface::Empty,
	Frame<Interface::Destructible>,
	Frame<Interface::Rectangular, CoordinateType>,
	Frame<Interface::Containing>,
	Frame<Interface::Movable>,
	Frame<Interface::UniformlyBordered, CoordinateType>,
	Frame<Interface::Textual, TextType>,
	Frame<Interface::SizedText>,
	Frame<Interface::Selectable>,
	Frame<Interface::Highlightable>,
	Frame<Interface::MultiPart, std::unique_ptr<      IShapePart<      CoordinateType>>,
								std::unique_ptr<const IShapePart<const CoordinateType>>>,
	Frame<Interface::Renderable>
>;

template<typename RectangleType, typename Margin, typename TextType = std::string>
using ControlBase =	FrameStack<
	IControl<typename RectangleType::coordinate_type, TextType>,
	Frame<Rectangular, RectangleType>,
	Frame<UniformlyBordered>,
	Frame<Selectable, IControl<typename RectangleType::coordinate_type, TextType>>,
	Frame<Highlightable>,
	Frame<Movable>,
	Frame<Textual, TextType>,
	Frame<SizedText, FunctionObjects::GlutStrokeFontEngine>,
	Frame<BoxedAdaptableSizeText, Margin>,
	Frame<MultiPartBorderedRectangle, ConcreteReturnTypesBuilder<ControlPart, typename RectangleType::coordinate_type>>
>;

template<typename RectangleType, typename BorderSize, typename Margin, typename TextType = std::string>
using Control = FrameStack<
	ControlBase<RectangleType, Margin, TextType>,
	Condition<
		FunctionObjects::Selected,
		Sequence<
			Frame<Colorblind::FilledRectangle>,
			FrameStack<
				Frame<Colorblind::BoxedText, FunctionObjects::Textual>,
				Frame<Colorblind::InversedColor>>>,
		Condition<
			FunctionObjects::Highlighted,
			Sequence<
				Frame<Colorblind::BorderedRectangle, BorderSize>,
				Frame<Colorblind::BoxedText, FunctionObjects::Textual>>,
			Sequence<
				Frame<Colorblind::BorderedRectangle, std::ratio<0>>,
				Frame<Colorblind::BoxedText, FunctionObjects::Textual>>
		>
	>
>;

template<typename RectangleType, typename BorderSize, typename Margin, typename LineSpacing, typename TextType = std::string>
using Paragraph = FrameStack<
	ControlBase<RectangleType, Margin, TextType>,
	Condition<
		FunctionObjects::Selected,
		Sequence<
			Frame<Colorblind::FilledRectangle>,
			FrameStack<
				Frame<Colorblind::BoxedParagraph, FunctionObjects::Textual, LineSpacing>,
				Frame<Colorblind::InversedColor>>>,
		Condition<
			FunctionObjects::Highlighted,
			Sequence<
				Frame<Colorblind::BorderedRectangle, BorderSize>,
				Frame<Colorblind::BoxedParagraph, FunctionObjects::Textual, LineSpacing>>,
			Sequence<
				Frame<Colorblind::BorderedRectangle, std::ratio<0>>,
				Frame<Colorblind::BoxedParagraph, FunctionObjects::Textual, LineSpacing>>
		>
	>
>;

template<typename RectangleType, typename Margin, typename TextType = std::string>
using Label = FrameStack<
	RectangleType,
	Frame<Movable>,
	Frame<Textual, TextType>,
	Frame<SizedText, FunctionObjects::GlutStrokeFontEngine>,
	Frame<BoxedAdaptableSizeText, Margin>,
	Sequence<
		Frame<Colorblind::FilledRectangle>,
		FrameStack<
			Frame<Colorblind::BoxedText, FunctionObjects::Textual>,
			Frame<Colorblind::InversedColor>>>
>;

BOOST_AUTO_TEST_CASE(Test_Button)
{
	Button<geometry::Rectangle<int>,std::ratio<10>,std::ratio<11>> button(10,"button",1,1,2,5,7);
	BOOST_CHECK_EQUAL(button.left(), 1);
	BOOST_CHECK_EQUAL(button.bottom(), 2);
	BOOST_CHECK_EQUAL(button.right(), 5);
	BOOST_CHECK_EQUAL(button.top(), 7);
	BOOST_CHECK_EQUAL(button.borderSize(), 1);
	BOOST_CHECK_EQUAL(button.text(), "button");
	BOOST_CHECK_EQUAL(button.textHeight(), 10);
	BOOST_CHECK_EQUAL(button.pressed(), false);
	BOOST_CHECK_EQUAL(button.highlighted(), false);
	button.depress().press().depress().press().borderSize() = 2;
	BOOST_CHECK_EQUAL(button.pressed(), true);
	BOOST_CHECK_EQUAL(button.borderSize(), 2);
	button.dehighlight().highlight().dehighlight().highlight().borderSize() = 3;
	BOOST_CHECK_EQUAL(button.highlighted(), true);
	BOOST_CHECK_EQUAL(button.borderSize(), 3);
	button.text() = "a";
	BOOST_CHECK_EQUAL(button.text(), "a");
	button.textHeight() = 12;
	BOOST_CHECK_EQUAL(button.textHeight(), 12);
	// TODO: check textWidth. What is the correct result?

	decltype(button) buttonCopy = button;
	BOOST_CHECK_EQUAL(buttonCopy.left(), 1);
	BOOST_CHECK_EQUAL(buttonCopy.bottom(), 2);
	BOOST_CHECK_EQUAL(buttonCopy.right(), 5);
	BOOST_CHECK_EQUAL(buttonCopy.top(), 7);
	BOOST_CHECK_EQUAL(buttonCopy.borderSize(), 3);
	BOOST_CHECK_EQUAL(buttonCopy.text(), "a");
	BOOST_CHECK_EQUAL(buttonCopy.textHeight(), 12);
	BOOST_CHECK_EQUAL(buttonCopy.pressed(), true);
	BOOST_CHECK_EQUAL(buttonCopy.highlighted(), true);

	decltype(button) buttonMove = std::move(button);
	BOOST_CHECK_EQUAL(buttonMove.left(), 1);
	BOOST_CHECK_EQUAL(buttonMove.bottom(), 2);
	BOOST_CHECK_EQUAL(buttonMove.right(), 5);
	BOOST_CHECK_EQUAL(buttonMove.top(), 7);
	BOOST_CHECK_EQUAL(buttonMove.borderSize(), 3);
	BOOST_CHECK_EQUAL(buttonMove.text(), "a");
	BOOST_CHECK_EQUAL(buttonMove.textHeight(), 12);
	BOOST_CHECK_EQUAL(buttonMove.pressed(), true);
	BOOST_CHECK_EQUAL(buttonMove.highlighted(), true);

	// Construct with some arguments missing
	decltype(button) b1;
	BOOST_CHECK_EQUAL(b1.textHeight(), 0);
	BOOST_CHECK_EQUAL(b1.text(), "");
	BOOST_CHECK_EQUAL(b1.pressed(), false);
	BOOST_CHECK_EQUAL(b1.highlighted(), false);
	BOOST_CHECK_EQUAL(b1.borderSize(), 0);

	decltype(button) b2("OK");
	BOOST_CHECK_EQUAL(b2.textHeight(), 0);
	BOOST_CHECK_EQUAL(b2.text(), "OK");
	BOOST_CHECK_EQUAL(b2.pressed(), false);
	BOOST_CHECK_EQUAL(b2.highlighted(), false);
	BOOST_CHECK_EQUAL(b2.borderSize(), 0);

	decltype(button) b3("OK",true);
	BOOST_CHECK_EQUAL(b3.textHeight(), 0);
	BOOST_CHECK_EQUAL(b3.text(), "OK");
	BOOST_CHECK_EQUAL(b3.highlighted(), true);
	BOOST_CHECK_EQUAL(b3.pressed(), false);
	BOOST_CHECK_EQUAL(b3.borderSize(), 0);

	decltype(button) b4("OK",true,true);
	BOOST_CHECK_EQUAL(b4.textHeight(), 0);
	BOOST_CHECK_EQUAL(b4.text(), "OK");
	BOOST_CHECK_EQUAL(b4.pressed(), true);
	BOOST_CHECK_EQUAL(b4.highlighted(), true);
	BOOST_CHECK_EQUAL(b4.borderSize(), 0);

	decltype(button) b5(10,"OK",true,true);
	BOOST_CHECK_EQUAL(b5.textHeight(), 10);
	BOOST_CHECK_EQUAL(b5.text(), "OK");
	BOOST_CHECK_EQUAL(b5.pressed(), true);
	BOOST_CHECK_EQUAL(b5.highlighted(), true);
	BOOST_CHECK_EQUAL(b5.borderSize(), 0);

	decltype(button) b6(10,"OK",true,true,2);
	BOOST_CHECK_EQUAL(b6.textHeight(), 10);
	BOOST_CHECK_EQUAL(b6.text(), "OK");
	BOOST_CHECK_EQUAL(b6.pressed(), true);
	BOOST_CHECK_EQUAL(b6.highlighted(), true);
	BOOST_CHECK_EQUAL(b6.borderSize(), 2);

	decltype(button) b7("OK",true,true,2);
	BOOST_CHECK_EQUAL(b7.textHeight(), 0);
	BOOST_CHECK_EQUAL(b7.text(), "OK");
	BOOST_CHECK_EQUAL(b7.pressed(), true);
	BOOST_CHECK_EQUAL(b7.highlighted(), true);
	BOOST_CHECK_EQUAL(b7.borderSize(), 2);

	decltype(button) b8(10,"OK",2);
	BOOST_CHECK_EQUAL(b8.textHeight(), 10);
	BOOST_CHECK_EQUAL(b8.text(), "OK");
	BOOST_CHECK_EQUAL(b8.pressed(), false);
	BOOST_CHECK_EQUAL(b8.highlighted(), false);
	BOOST_CHECK_EQUAL(b8.borderSize(), 2);

	decltype(button) b9(10,"OK");
	BOOST_CHECK_EQUAL(b9.textHeight(), 10);
	BOOST_CHECK_EQUAL(b9.text(), "OK");
	BOOST_CHECK_EQUAL(b9.pressed(), false);
	BOOST_CHECK_EQUAL(b9.highlighted(), false);
	BOOST_CHECK_EQUAL(b9.borderSize(), 0);

	decltype(button) b10("OK",2);
	BOOST_CHECK_EQUAL(b10.textHeight(), 0);
	BOOST_CHECK_EQUAL(b10.text(), "OK");
	BOOST_CHECK_EQUAL(b10.pressed(), false);
	BOOST_CHECK_EQUAL(b10.highlighted(), false);
	BOOST_CHECK_EQUAL(b10.borderSize(), 2);

	// Instantiate with other coordinate types
	Button<geometry::Rectangle<float>,std::ratio<10>,std::ratio<11>> b02(10.0f,"button",1.0f,1.0f,2.0f,5.0f,7.0f);
	BOOST_CHECK_EQUAL(b02.textHeight(), 10.0f);
	BOOST_CHECK_EQUAL(b02.text(), "button");
	BOOST_CHECK_EQUAL(b02.pressed(), false);
	BOOST_CHECK_EQUAL(b02.highlighted(), false);
	BOOST_CHECK_EQUAL(b02.borderSize(), 1.0f);
	BOOST_CHECK_EQUAL(b02.left(), 1.0f);
	BOOST_CHECK_EQUAL(b02.bottom(), 2.0f);
	BOOST_CHECK_EQUAL(b02.right(), 5.0f);
	BOOST_CHECK_EQUAL(b02.top(), 7.0f);
	Button<geometry::Rectangle<double>,std::ratio<10>,std::ratio<11>> b03(10.0,"button",1.0,1.0,2.0,5.0,7.0);
	BOOST_CHECK_EQUAL(b03.textHeight(), 10.0);
	BOOST_CHECK_EQUAL(b03.text(), "button");
	BOOST_CHECK_EQUAL(b03.pressed(), false);
	BOOST_CHECK_EQUAL(b03.highlighted(), false);
	BOOST_CHECK_EQUAL(b03.borderSize(), 1.0);
	BOOST_CHECK_EQUAL(b03.left(), 1.0);
	BOOST_CHECK_EQUAL(b03.bottom(), 2.0);
	BOOST_CHECK_EQUAL(b03.right(), 5.0);
	BOOST_CHECK_EQUAL(b03.top(), 7.0);
	Button<geometry::Rectangle<double>,std::ratio<10>,std::ratio<11>> b04(10.0,"button",true,true,1.0,1.0,2.0,5.0,7.0);
	BOOST_CHECK_EQUAL(b04.textHeight(), 10.0);
	BOOST_CHECK_EQUAL(b04.text(), "button");
	BOOST_CHECK_EQUAL(b04.pressed(), true);
	BOOST_CHECK_EQUAL(b04.highlighted(), true);
	BOOST_CHECK_EQUAL(b04.borderSize(), 1.0);
	BOOST_CHECK_EQUAL(b04.left(), 1.0);
	BOOST_CHECK_EQUAL(b04.bottom(), 2.0);
	BOOST_CHECK_EQUAL(b04.right(), 5.0);
	BOOST_CHECK_EQUAL(b04.top(), 7.0);

	// Construct with l-values
	float c1=5, c2=6, c3=7, c4=8, b=1, h=10;
	std::string name("cancel");
	Button<geometry::Rectangle<float>,std::ratio<10>,std::ratio<11>> b05(h,name,b,c1,c2,c3,c4);
	BOOST_CHECK_EQUAL(b05.left(), c1);
	BOOST_CHECK_EQUAL(b05.bottom(), c2);
	BOOST_CHECK_EQUAL(b05.right(), c3);
	BOOST_CHECK_EQUAL(b05.top(), c4);
	BOOST_CHECK_EQUAL(b05.borderSize(), b);
	BOOST_CHECK_EQUAL(b05.text(), name);
	BOOST_CHECK_EQUAL(b05.textHeight(), h);
	BOOST_CHECK_EQUAL(b05.pressed(), false);
	BOOST_CHECK_EQUAL(b05.highlighted(), false);

//	Button<geometry::Rectangle<float>,std::ratio<10>,std::ratio<11>> b06(h,"button",c1,c2,c3,c4); // currently compile-time error
} // end test case

BOOST_AUTO_TEST_CASE(Test_Label)
{
	Label<geometry::Rectangle<int>,std::ratio<10>> label(10,"label",2,3,4,5);
	BOOST_CHECK_EQUAL(label.left(), 2);
	BOOST_CHECK_EQUAL(label.bottom(), 3);
	BOOST_CHECK_EQUAL(label.right(), 4);
	BOOST_CHECK_EQUAL(label.top(), 5);
	BOOST_CHECK_EQUAL(label.text(), "label");
	BOOST_CHECK_EQUAL(label.textHeight(), 10);

	decltype(label) labelCopy = label;
	BOOST_CHECK_EQUAL(labelCopy.left(), 2);
	BOOST_CHECK_EQUAL(labelCopy.bottom(), 3);
	BOOST_CHECK_EQUAL(labelCopy.right(), 4);
	BOOST_CHECK_EQUAL(labelCopy.top(), 5);
	BOOST_CHECK_EQUAL(labelCopy.text(), "label");
	BOOST_CHECK_EQUAL(labelCopy.textHeight(), 10);

	decltype(label) labelMove = std::move(label);
	BOOST_CHECK_EQUAL(labelMove.left(), 2);
	BOOST_CHECK_EQUAL(labelMove.bottom(), 3);
	BOOST_CHECK_EQUAL(labelMove.right(), 4);
	BOOST_CHECK_EQUAL(labelMove.top(), 5);
	BOOST_CHECK_EQUAL(labelMove.text(), "label");
	BOOST_CHECK_EQUAL(labelMove.textHeight(), 10);
} // end test case

BOOST_AUTO_TEST_CASE(Test_Paragraph)
{
	Paragraph<geometry::Rectangle<int>,std::ratio<10>,std::ratio<11>,std::ratio<5>> paragraph(11,"paragraph",1,3,4,5,6);
	BOOST_CHECK_EQUAL(paragraph.left(), 3);
	BOOST_CHECK_EQUAL(paragraph.bottom(), 4);
	BOOST_CHECK_EQUAL(paragraph.right(), 5);
	BOOST_CHECK_EQUAL(paragraph.top(), 6);
	BOOST_CHECK_EQUAL(paragraph.borderSize(), 1);
	BOOST_CHECK_EQUAL(paragraph.text(), "paragraph");
	BOOST_CHECK_EQUAL(paragraph.textHeight(), 11);
	BOOST_CHECK_EQUAL(paragraph.selected(), false);
	BOOST_CHECK_EQUAL(paragraph.highlighted(), false);
	paragraph.highlight().select();

	decltype(paragraph) paragraphCopy = paragraph;
	BOOST_CHECK_EQUAL(paragraphCopy.left(), 3);
	BOOST_CHECK_EQUAL(paragraphCopy.bottom(), 4);
	BOOST_CHECK_EQUAL(paragraphCopy.right(), 5);
	BOOST_CHECK_EQUAL(paragraphCopy.top(), 6);
	BOOST_CHECK_EQUAL(paragraphCopy.borderSize(), 1);
	BOOST_CHECK_EQUAL(paragraphCopy.text(), "paragraph");
	BOOST_CHECK_EQUAL(paragraphCopy.textHeight(), 11);
	BOOST_CHECK_EQUAL(paragraphCopy.selected(), true);
	BOOST_CHECK_EQUAL(paragraphCopy.highlighted(), true);

	decltype(paragraph) paragraphMove = std::move(paragraph);
	BOOST_CHECK_EQUAL(paragraphMove.left(), 3);
	BOOST_CHECK_EQUAL(paragraphMove.bottom(), 4);
	BOOST_CHECK_EQUAL(paragraphMove.right(), 5);
	BOOST_CHECK_EQUAL(paragraphMove.top(), 6);
	BOOST_CHECK_EQUAL(paragraphMove.borderSize(), 1);
	BOOST_CHECK_EQUAL(paragraphMove.text(), "paragraph");
	BOOST_CHECK_EQUAL(paragraphMove.textHeight(), 11);
	BOOST_CHECK_EQUAL(paragraphMove.selected(), true);
	BOOST_CHECK_EQUAL(paragraphMove.highlighted(), true);
} // end test case

BOOST_AUTO_TEST_CASE(Test_Control)
{
	Control<geometry::Rectangle<int>,std::ratio<10>,std::ratio<11>> control(15,"control",2,4,3,2,1);
	BOOST_CHECK_EQUAL(control.left(), 4);
	BOOST_CHECK_EQUAL(control.bottom(), 3);
	BOOST_CHECK_EQUAL(control.right(), 2);
	BOOST_CHECK_EQUAL(control.top(), 1);
	BOOST_CHECK_EQUAL(control.borderSize(), 2);
	BOOST_CHECK_EQUAL(control.text(), "control");
	BOOST_CHECK_EQUAL(control.textHeight(), 15);
	BOOST_CHECK_EQUAL(control.selected(), false);
	BOOST_CHECK_EQUAL(control.highlighted(), false);
	control.highlight().select();

	decltype(control) controlCopy = control;
	BOOST_CHECK_EQUAL(controlCopy.left(), 4);
	BOOST_CHECK_EQUAL(controlCopy.bottom(), 3);
	BOOST_CHECK_EQUAL(controlCopy.right(), 2);
	BOOST_CHECK_EQUAL(controlCopy.top(), 1);
	BOOST_CHECK_EQUAL(controlCopy.borderSize(), 2);
	BOOST_CHECK_EQUAL(controlCopy.text(), "control");
	BOOST_CHECK_EQUAL(controlCopy.textHeight(), 15);
	BOOST_CHECK_EQUAL(controlCopy.selected(), true);
	BOOST_CHECK_EQUAL(controlCopy.highlighted(), true);

	decltype(control) controlMove = std::move(control);
	BOOST_CHECK_EQUAL(controlMove.left(), 4);
	BOOST_CHECK_EQUAL(controlMove.bottom(), 3);
	BOOST_CHECK_EQUAL(controlMove.right(), 2);
	BOOST_CHECK_EQUAL(controlMove.top(), 1);
	BOOST_CHECK_EQUAL(controlMove.borderSize(), 2);
	BOOST_CHECK_EQUAL(controlMove.text(), "control");
	BOOST_CHECK_EQUAL(controlMove.textHeight(), 15);
	BOOST_CHECK_EQUAL(controlMove.selected(), true);
	BOOST_CHECK_EQUAL(controlMove.highlighted(), true);
} // end test case
