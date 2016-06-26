#ifndef ROTOOLS_H
#define ROTOOLS_H

#define SAFE_DELETE( x ) if ( x ) \
{\
    delete x;\
    x=nullptr;\
    }

#define SAFE_DELETE_ARRAY(x) if ( x )\
{\
    delete []x;\
    x=nullptr;\
    }

#endif // ROTOOLS_H
