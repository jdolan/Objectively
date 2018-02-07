// Provides wrappers for the POSIX dl* functions

/*
The <dlfcn.h> header defines at least the following macros for use in the construction of a dlopen() mode argument:
RTLD_LAZY
Relocations are performed at an implementation-dependent time.
RTLD_NOW
Relocations are performed when the object is loaded.
RTLD_GLOBAL
All symbols are available for relocation processing of other modules.
RTLD_LOCAL
All symbols are not made available for relocation processing by other modules.
The header <dlfcn.h> declares the following functions which may also be defined as macros. Function prototypes must be provided for use with an ISO C compiler.


void  *dlopen(const char *, int);
void  *dlsym(void *, const char *);
int    dlclose(void *);
char  *dlerror(void);*/

#ifdef DLFCN_INCLUDED
void *dlopen(const char *file, int mode);
int   dlclose(void *handle);
void *dlsym(void *handle, const char *name);
char *dlerror(void);
#else
#include <windows.h>
#include <psapi.h>

static HMODULE _runningModule(void) {
	static HMODULE myModule = NULL;

	if (myModule = NULL)
		myModule = GetModuleHandle(NULL);

	return myModule;
}

static HANDLE _runningProcess(void) {
	static HANDLE myProcess = NULL;

	if (myProcess == NULL)
		myProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, GetCurrentProcessId());

	return myProcess;
}

void *dlopen(const char *file, int mode) {
	if (file == NULL) {
		return _runningModule();
	}

	return LoadLibrary(file);
}

int dlclose(void *handle) {
	if (handle == _runningModule())
		return;

	return FreeLibrary((HMODULE) handle);
}

static void *dlsym_global(const char *name) {
	static HMODULE hMods[1024];
	static DWORD sizehMods;

	if (!EnumProcessModules(_runningProcess(), hMods, sizeof(hMods), &sizehMods))
		return NULL;

	void *address = NULL;
	sizehMods = (sizehMods / sizeof(HMODULE));

	for (DWORD i = 0; i < sizehMods; i++) {
		if ((address = GetProcAddress(hMods[i], name)))
			return address;
	}

	return NULL;
}

void *dlsym(void *handle, const char *name) {
	if (handle == _runningModule()) {
		return dlsym_global(name);
	}

	return GetProcAddress((HMODULE) handle, name);
}

char *dlerror(void) {
	return "Errors, eh?";
}
#endif

#define DLFCN_INCLUDED
