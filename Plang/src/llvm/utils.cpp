#ifndef LLVM_UTILS
#define LLVM_UTILS

#define unreachable() printf("UNREACHABLE: node unimplemented in function %s\n", __func__)

template<typename U, typename T>
inline U* dcast(T* d) { // dynamic_cast
	return dynamic_cast<U*>(d);
}
#define BUILD_STRING_FROM_VARARG(__fmt, __res)		\
	va_list args;									\
	va_start(args, __fmt);							\
	size_t n = 1 + vsnprintf(NULL, 0, __fmt, args);	\
	va_end(args);									\
	char* (__res)= (char*)xmalloc(n);				\
	va_start(args, __fmt);							\
	vsnprintf((__res), n, __fmt, args);				\
	va_end(args);


#endif/*LLVM_UTILS*/