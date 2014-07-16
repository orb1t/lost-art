#ifndef EIGEN_RATIONAL_INTERFACE_CODE_H
#define EIGEN_RATIONAL_INTERFACE_CODE_H

#include <boost/rational.hpp>
#include <Eigen/Dense>

namespace Eigen
{
	template<typename I>
	struct NumTraits<boost::rational<I>>
	{
		typedef boost::rational<I> Real;
		typedef boost::rational<I> NonInteger; // perhaps double would be better?
		typedef boost::rational<I> Nested;

		enum{
			IsComplex = 0,
			IsInteger = 0,
			ReadCost = 2,
			AddCost = 10,
			MulCost = 10,
			IsSigned = std::numeric_limits<I>::is_signed,
			RequireInitialization = 1,
		}; // end enum

		static Real epsilon()
		{
			return boost::rational<I>(std::numeric_limits<I>::max(),std::numeric_limits<I>::max()-1);
		} // end function epsilon

		static Real dummy_precision()
		{
			return epsilon()*10000;
		} // end function dummy_precision

		static Real highest()
		{
			return boost::rational<I>(std::numeric_limits<I>::max(),1);
		} // end function highest

		static Real lowest()
		{
			return boost::rational<I>(std::numeric_limits<I>::min(),1);
		} // end function lowest
	}; // end struct NumTraits

	// Eigen didn't document it but I still needed to specialize this class...
	namespace internal
	{
		template<typename I, typename NewType>
		struct cast_impl<boost::rational<I>,NewType>
		{
			static inline NewType run(const boost::rational<I>& x)
			{
				return boost::rational_cast<NewType>(x);
			} // end function run
		}; // end struct cast_impl
	} // end namespace internal
} // end namespace Eigen


namespace boost
{
	template<typename I>
	double log(boost::rational<I> r)
	{
		return std::log(boost::rational_cast<double>(r));
	} // end function log
} // end namespace boost

#endif /* EIGEN_RATIONAL_INTERFACE_CODE_H */