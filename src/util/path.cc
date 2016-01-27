

#include "../global.h"
#include "path.h"



namespace def {
namespace util {




#ifdef WINDOWS
	char Path::D = '\\';
#else
	char Path::D = '/';
#endif

string Path::m_cwd = "";



} // --end-- namespace util
} // --end-- namespace def

