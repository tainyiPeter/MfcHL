#ifndef _STD_HELPER_MACRO_H
#define _STD_HELPER_MACRO_H

#include <vector>
#include <list>
#include <map>
#include <stack>
#include <algorithm>
#include <functional>

#define DECALARE_STDCONTAINER(name, ctype, ...)	\
	typedef ctype<__VA_ARGS__>		name;	\
	typedef name::iterator			name##ITR;	\
	typedef name::reverse_iterator	name##R_ITR;	\
	typedef name::const_iterator	CONST_##name##ITR;	\
	typedef name::const_reverse_iterator CONST_##name##RITR;

#define DECALARE_SINGLE_PARAM_CONTAINER(ctype, vtype, name)	DECALARE_STDCONTAINER(name, ctype, vtype)
#define DECALARE_TWO_PARAM_CONTAINER(ctype, vtype1, vtype2, name)	DECALARE_STDCONTAINER(name, ctype, vtype1, vtype2)

#define DECLARE_VECTOR(type, name)		DECALARE_STDCONTAINER(name, std::vector, type)
#define DECLARE_LIST(type, name)		DECALARE_STDCONTAINER(name, std::list, type)
#define DECLARE_MAP(k, v, name)			DECALARE_STDCONTAINER(name, std::map, k, v) \
	typedef name::value_type		name##VALUETYPE;
#define DECLARE_MULTIMAP(k, v, name)	DECALARE_STDCONTAINER(name, std::multimap, k, v)\
	typedef name::value_type		name##VALUETYPE;

#define DECLARE_PAIR(f, s, name)		typedef std::pair<f, s>	name;

#define SIMPLE_FIND_OBJECT(c, o)		std::find(c.begin(), c.end(), o)

#define SIMPLE_FIND_OBJECT_EXIST(c, o)	(std::find(c.begin(), c.end(), o)!=c.end())
#define SIMPLE_FIND_OBJECT_NOTEXIST(c, o)	(std::find(c.begin(), c.end(), o)==c.end())

#define SIMPLE_FIND_OBJECTEX(c, o, f)	std::find(c.begin(), c.end(), o, f)

#define SIMPLE_FOR_EACH(c, o)			std::for_each(c.begin(), c.end(), o)

template <class T> void ClearMapContainer(T& c)
{
	for (typename T::iterator itr = c.begin(); itr != c.end(); ++itr) 
	{	
		delete itr->second;	
	}

	c.clear();	
}

template <class T> void ClearContainer(T& c)
{
	for (typename T::iterator itr = c.begin(); itr != c.end(); ++itr)
	{	
		delete *itr;	
	}

	c.clear();	
}

#endif // _STD_HELPER_MACRO_H