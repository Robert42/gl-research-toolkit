// ======== C++ ================================================================
#if defined(__GNUC__)

#ifdef out
#undef out
#endif

#ifdef in
#undef in
#endif

#ifdef inout
#undef inout
#endif

#undef xyz
#undef xyxy
#undef xx
#undef xxx
#undef xxxx
#undef yy
#undef yyy
#undef yyyy
#undef yzw
#undef yzx
#undef zxy
#undef xz
#undef yw
#undef yz
#undef xy
#undef zw
#undef xyzw
#undef yzwx
#undef zwxy
#undef wxyz

} // namespace glsl
} // namespace glrt

// ======== GLSL ===============================================================
#else

#define not_ not

#ifdef out
#undef out
#endif

#ifdef inout
#undef inout
#endif

#endif
