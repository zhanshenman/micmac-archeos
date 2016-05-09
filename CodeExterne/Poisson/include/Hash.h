#ifndef HASH_INCLUDED
#define HASH_INCLUDED

// MPD _MSC_VER == _MSC_VER_2015 est vrai meme sous Gnu/Linux ...
#if  ((defined __APPLE__) && (defined __clang__) && (__clang_major__>=5)) ||( ELISE_windows && (_MSC_VER == _MSC_VER_2015))
	#include <unordered_map>
	#define hash_map std::unordered_map
#elif _WIN32
	#ifndef __GNUC__
		//~ #include <hash_map>
		//~ using stdext::hash_map;
		#include <unordered_map>
		#define hash_map std::unordered_map
	#elif __GNUC__ < 3
		#include <hash_map>
	#else
		#include <ext/hash_map>
		#if __GNUC_MINOR__ == 0
				using namespace std;       // GCC 3.0
		#else
				using namespace __gnu_cxx; // GCC >= 3.1
		#endif
	#endif
#else // !WIN32
	#include <tr1/unordered_map>
	#define hash_map std::tr1::unordered_map
#endif // WIN32

#endif // HASH_INCLUDED


