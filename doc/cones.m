% execute with `clear && octave -q cones.m`


% Prefer high precision output
format long

function r = radians(degrees)
  r = degrees * pi / 180;
end

function m = rotationMatrixX(a)
  m = [1 0 0 0; ...
       0 cos(a) -sin(a) 0; ...
       0 sin(a)  cos(a) 0; ...
       0 0 0 1];
endfunction

function m = rotationMatrixY(a)
  m = [cos(a) 0 sin(a) 0; ...
       0 1 0 0; ...
       -sin(a) 0 cos(a) 0; ...
       0 0 0 1];
endfunction

function m = rotationMatrixZ(a)
  m = [cos(a) -sin(a) 0 0; ...
       sin(a) cos(a) 0 0; ...
       0 0 1 0; ...
       0 0 0 1];
endfunction

function cone(openingAngle, matrix=eye(4))
  disp("#### next-cone")
  disp("half_angle: "),disp(openingAngle * 0.5);
  disp("direction: "),disp(matrix(1:3,3));
end


angle = radians(60);

cone(angle);
