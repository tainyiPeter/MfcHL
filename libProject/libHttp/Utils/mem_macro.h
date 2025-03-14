#ifndef _MEMORY_HELPER_MACRO_H
#define _MEMORY_HELPER_MACRO_H

#define INIT_LOCK(name)			::InitializeCriticalSection(&name)
#define DEL_LOCK(name)			::DeleteCriticalSection(&name)
#define ENTER_LOCK(name)		CAutoLock lock(&name)

#define CLOSE_HANDLE(h)	\
	if (h != NULL) {	\
	CloseHandle(h);	\
	h = NULL; }

#define CLOSE_FILE_HANDLE(h)	\
	if (h != INVALID_HANDLE_VALUE) {	\
	CloseHandle(h);	\
	h = INVALID_HANDLE_VALUE; }

#define SIMPLE_ZEROMEMORY(obj)		\
	ZeroMemory(&obj, sizeof(obj))

#define SIMPLE_ZEROMEMORY_POINTER(p)		\
	ZeroMemory(p, sizeof((*p)))

#define SIMPLE_COPYOBJECT(obj1, obj2)	\
	memcpy_s(&obj1, sizeof(obj1), &obj2, sizeof(obj1))

#define DELETE_PTR(p)	\
	delete p;	\
	p = NULL;
#define DELETE_PTRA(p)	\
	delete[]p;	\
	p = NULL;


#define DELETE_CTRL(p)	\
	if(p && p->GetSafeHwnd())\
	{\
		p->DestroyWindow();\
	}\
	delete p;\
	p = NULL;

#define MALLOC_PTR(type, count)	(type*)malloc(sizeof(type) * count)

#define FREE_PTR(in)	\
	free(in);	\
	in = NULL;

#define G_FREE_OBJ(pObj)\
	if (pObj)\
	{\
	pObj->Close();\
	delete pObj;\
	pObj = NULL;\
	}

#define  CHECK_VALID_PRT(ptr) \
if(NULL == ptr)\
{\
	return ;\
}


#define  CHECK_VALID_PRT_RETURN(ptr, retrunVal) \
if(NULL == ptr)\
{\
	return retrunVal;\
}


#define VECTOR_MERGE(vectDest, vectorOrg, bClearFirt) \
if(bClearFirt) \
{\
	vectDest.clear();\
}\
if(FALSE == vectorOrg.empty())\
{\
	vectDest.insert(vectDest.end(), vectorOrg.begin(), vectorOrg.end());\
}

#endif // _MEMORY_HELPER_MACRO_H