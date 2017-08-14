#ifndef PORTABILITY_H
#define PORTABILITY_H

#if defined(_MSC_VER) && (_MSC_VER <= 1200)

#include <functional>

namespace std {

template<class _R, class _Ty, class _A> inline
	mem_fun1_t<_R, _Ty, _A> mem_fun(_R (_Ty::*_Pm)(_A))
	{return (mem_fun1_t<_R, _Ty, _A>(_Pm)); };

}

#endif

#endif // PORTABILITY_H
