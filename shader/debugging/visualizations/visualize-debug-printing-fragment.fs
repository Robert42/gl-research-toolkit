#version 450 core

out vec4 color;

void draw_cross(vec2 position, float line_width, float line_length)
{
  line_width *= 0.5;
  line_length *= 0.5;
  
  if(any(lessThanEqual(gl_FragCoord.xy, position-line_length)))
    discard;
  if(any(greaterThanEqual(gl_FragCoord.xy, position+line_length)))
    discard;
    
  if(!any(greaterThanEqual(gl_FragCoord.xy, position-line_width) && lessThanEqual(gl_FragCoord.xy, position+line_width) ))
    discard;
}

void main()
{
  draw_cross(vec2(100, 100), 2, 16);
  
  color = vec4(1, 0, 1, 1);
}
