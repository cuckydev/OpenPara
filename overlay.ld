. = __bss_end;
. = ALIGN(16);
__heap_base = .;

#define DEFINE_OVERLAY(name)                        \
.name                                               \
{                                                   \
	KEEP(src/name/?*.o(.text))                      \
	__##name##_ctor = .;                            \
	KEEP(src/name/?*.o(.text.startup._GLOBAL__*))   \
	KEEP(src/name/?*.o(.text.*))                    \
	KEEP(src/name/?*.o(.rodata*))                   \
	KEEP(src/name/?*.o(.sdata*))                    \
	KEEP(src/name/?*.o(.data*))                     \
	KEEP(src/name/?*.o(.ctors))                     \
	. = ALIGN(4);                                   \
	__##name##_end = .;                             \
}

SECTIONS
{
	OVERLAY __heap_base : NOCROSSREFS SUBALIGN(4)
	{
		DEFINE_OVERLAY(S1)
	}
}
