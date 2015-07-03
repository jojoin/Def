

#include "../global.h"
#include "path.h"



namespace def {
namespace util {




#ifdef WINDOWS
	char Path::D = '\\';
#else
	char Path::D = '/';
#endif



} // --end-- namespace util
} // --end-- namespace def

