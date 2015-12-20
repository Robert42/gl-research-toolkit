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

#undef xyxy
#undef zw

} // namespace glsl
} // namespace glrt

// ======== GLSL ===============================================================
#else

#ifdef out
#undef out
#endif

#ifdef inout
#undef inout
#endif

#endif
