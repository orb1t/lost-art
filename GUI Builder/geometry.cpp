#include "geometry.h"

#include <cassert>

namespace geometry
{
	void runTestSuite()
	{
		// test Rectangle
		Rectangle<int> recti(-2,5,3,8);
		assert(recti.left() == -2);
		assert(recti.right() == 3);
		assert(recti.bottom() == 5);
		assert(recti.top() == 8);
		recti.sides()[size_t(RectangleSide::RIGHT)] = 0;
		assert(recti.right() == 0);
		recti.side(geometry::Rectangle<int>::Side::BOTTOM) = 4;
		assert(recti.bottom() == 4);

		Rectangle<float> rectf(-2.0,5.0,3.0,8.0);
		assert(rectf.sides()[size_t(Rectangle<float>::Side::TOP)] == 8.0);

		// test RefRectangle
		int x = -1, y = 7;
		RefRectangle<int,true,false,false,true> refrecti(x,5,3,y);
		assert(refrecti.left() == -1);
		assert(refrecti.right() == 3);
		assert(refrecti.bottom() == 5);
		assert(refrecti.top() == 7);
		refrecti.side(RectangleSide::RIGHT) = 0;
		assert(refrecti.right() == 0);
		assert(x == -1);
		assert(y == 7);
		refrecti.side(geometry::RefRectangle<int,true,false,false,true>::Side::BOTTOM) = 4;
		assert(refrecti.bottom() == 4);
		assert(x == -1);
		assert(y == 7);
		refrecti.left() = 0;
		assert(refrecti.left() == 0);
		assert(x == 0);
		x = -3;
		assert(refrecti.left() == -3);
		refrecti.top() = 9;
		assert(refrecti.top() == 9);
		assert(y == 9);
		y = 10;
		assert(refrecti.top() == 10);

		RefRectangle<float> refrectf(-2.0,5.0,3.0,8.0);
		assert(refrectf.side(Rectangle<float>::Side::TOP) == 8.0);

	} // end function runTestSuite

} // end namespace geometry
