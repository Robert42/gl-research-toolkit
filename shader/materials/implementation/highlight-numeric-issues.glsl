void highlight_numeric_issues()
{
  float alpha = fragment_color.a;
  
  #if defined(SHOW_NAN) || defined(SHOW_INF)
  
  #if defined(SHOW_NAN)
    bvec4 should_highlight = isnan(fragment_color);
  #elif defined(SHOW_INF)
    bvec4 should_highlight = isinf(fragment_color);
  #endif
  
    if(any(should_highlight))
      fragment_color = vec4(1, 0, 1, 1);
    else
      fragment_color = vec4(0, 0, 0, alpha);
  #endif
}