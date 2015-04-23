#ifndef OS_INTF_HOLD_HH
#define OS_INTF_HOLD_HH

#ifdef WINTER_RAW

#define PRINT_SHARED_MEMORY_ACTIVITY ""

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
	extern void rcs_print_error(const char* fmt, ...);
	extern void rcs_puts(...);
	extern void rcs_print_debug(const char* fmt, ...);
#ifdef __cplusplus
}
#endif // __cplusplus

#endif // WINTER_RAW

#endif
