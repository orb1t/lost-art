#include "graphene.h"

#include <cassert>

namespace graphene
{
	void runTestSuite()
	{
		Controls::Button<geometry::Rectangle<int>,std::ratio<10>,std::ratio<11>> button(1,2,5,7,1,"button",10);

		assert(button.left() == 1);
		assert(button.bottom() == 2);
		assert(button.right() == 5);
		assert(button.top() == 7);
		assert(button.borderSize() == 1);
		assert(button.text() == "button");
		assert(button.textHeight() == 10);
		assert(button.pressed() == false);
		assert(button.highlighted() == false);
		button.depress().press().depress().press().borderSize() = 2;
		assert(button.pressed() == true);
		assert(button.borderSize() == 2);
		button.dehighlight().highlight().dehighlight().highlight().borderSize() = 3;
		assert(button.highlighted() == true);
		assert(button.borderSize() == 3);
		button.text() = "a";
		assert(button.text() == "a");
		button.textHeight() = 12;
		assert(button.textHeight() == 12);
		// TODO: check textWidth. What is the correct result?
	} // end function runTestSuite

} // end namespace graphene
