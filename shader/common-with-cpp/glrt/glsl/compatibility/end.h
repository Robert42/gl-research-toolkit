// ======== C++ ================================================================
#if defined(__GNUC__)

#ifdef out
#undef out
#endif

#ifdef inout
#undef inout
#endif

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