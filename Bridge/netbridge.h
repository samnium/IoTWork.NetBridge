
// --------------------------------------
//
// CONSTANTS
//
// --------------------------------------

#define TRUE    1
#define FALSE   0


// --------------------------------------
//
// DEBUGGING
//
// --------------------------------------

//#undef DEBUG

#if defined(DEBUG) && DEBUG > 0
#define DEBUG_PRINT(fmt, args...) fprintf(stderr, "DEBUG: %s:%d:%s(): " fmt, \
    __FILE__, __LINE__, __func__, ##args)
#else
#define DEBUG_PRINT(fmt, args...) do{ } while ( 0 )
#endif
