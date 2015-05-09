#ifndef INIHAL_H
#define INIHAL_H
#include "hal.h"
#include "emcmotcfg.h"

int check_ini_hal_items();
int ini_hal_init(void);
int ini_hal_init_pins(void);

/* not doing these fields (yet,ever?):
[TRAJ]COORDINATES
[TRAJ]AXES
[TRAJ]LINEAR_UNITS
[TRAJ]ANGULAR_UNITS
[TRAJ]HOME
[AXIS_n]TYPE
[AXIS_n]UNITS
[AXIS_n]HOME
[AXIS_n]HOME_OFFSET
[AXIS_n]HOME_SEARCH_VEL
[AXIS_n]HOME_LATCH_VEL
[AXIS_n]HOME_FINAL_VEL
[AXIS_n]HOME_IS_SHARED
[AXIS_n]HOME_USE_INDEX
[AXIS_n]HOME_IGNORE_LIMITS
[AXIS_n]HOME_SEQUENCE
[AXIS_n]VOLATILE_HOME
[AXIS_n]LOCKING_INDEXER
[AXIS_n]COMP_FILE_TYPE
[AXIS_n]COMP
*/
#define HAL_FIELDS \
    FIELD(hal_float_t,traj_default_velocity) \
    FIELD(hal_float_t,traj_max_velocity) \
    FIELD(hal_float_t,traj_default_acceleration) \
    FIELD(hal_float_t,traj_max_acceleration) \
\
    FIELD(hal_bit_t,traj_arc_blend_enable) \
    FIELD(hal_bit_t,traj_arc_blend_fallback_enable) \
    FIELD(hal_s32_t,traj_arc_blend_optimization_depth) \
    FIELD(hal_float_t,traj_arc_blend_gap_cycles) \
    FIELD(hal_float_t,traj_arc_blend_ramp_freq) \
    FIELD(hal_float_t,traj_arc_blend_tangent_kink_ratio) \
\
    ARRAY(hal_float_t,backlash,EMCMOT_MAX_JOINTS) \
    ARRAY(hal_float_t,min_limit,EMCMOT_MAX_JOINTS) \
    ARRAY(hal_float_t,max_limit,EMCMOT_MAX_JOINTS) \
    ARRAY(hal_float_t,max_velocity,EMCMOT_MAX_JOINTS) \
    ARRAY(hal_float_t,max_acceleration,EMCMOT_MAX_JOINTS) \
    ARRAY(hal_float_t,ferror,EMCMOT_MAX_JOINTS) \
    ARRAY(hal_float_t,min_ferror,EMCMOT_MAX_JOINTS) \

struct PTR {
    template<class T>
    struct field { typedef T *type; };
};

template<class T> struct NATIVE {};
template<> struct NATIVE<hal_bit_t> { typedef bool type; };
template<> struct NATIVE<hal_s32_t> { typedef rtapi_s32 type; };
template<> struct NATIVE<hal_u32_t> { typedef rtapi_u32 type; };
template<> struct NATIVE<hal_float_t> { typedef double type; };
struct VALUE {
    template<class T> struct field { typedef typename NATIVE<T>::type type; };
};

template<class T>
struct inihal_base
{
#define FIELD(t,f) typename T::template field<t>::type f;
#define ARRAY(t,f,n) typename T::template field<t>::type f[n];
HAL_FIELDS
#undef FIELD
#undef ARRAY
};

typedef inihal_base<PTR> ptr_inihal_data;
typedef inihal_base<VALUE> value_inihal_data;

#endif

