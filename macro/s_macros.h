#ifndef __S_MACROS_H__
#define __S_MACROS_H__

#if     __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 8)
#define S_GNUC_EXTENSION __extension__
#else
#define S_GNUC_EXTENSION
#endif

#if defined(__GNUC__) && (__GNUC__ > 2) && defined(__OPTIMIZE__)
#define _S_BOOLEAN_EXPR(expr)                   \
         S_GNUC_EXTENSION ({                            \
                    int _s_boolean_var_;                         \
                    if (expr)                                    \
                       _s_boolean_var_ = 1;                      \
                    else                                         \
                       _s_boolean_var_ = 0;                      \
                    _s_boolean_var_;                             \
                 })
#define S_LIKELY(expr) (__builtin_expect (_S_BOOLEAN_EXPR((expr)), 1))
#define S_UNLIKELY(expr) (__builtin_expect (_S_BOOLEAN_EXPR((expr)), 0))
#else
#define S_LIKELY(expr) (expr)
#define S_UNLIKELY(expr) (expr)
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#endif /* __S_MACROTS_H__ */
