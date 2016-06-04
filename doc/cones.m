% execute with `clear && octave -q cones.m`


% Prefer high precision output
format long

function r = radians(degrees)
  r = degrees * pi / 180;
end

function m = rotationMatrixX(a)
  a = radians(a);
  m = [1 0 0 0; ...
       0 cos(a) -sin(a) 0; ...
       0 sin(a)  cos(a) 0; ...
       0 0 0 1];
endfunction

function m = rotationMatrixY(a)
  a = radians(a);
  m = [cos(a) 0 sin(a) 0; ...
       0 1 0 0; ...
       -sin(a) 0 cos(a) 0; ...
       0 0 0 1];
endfunction

function m = rotationMatrixZ(a)
  a = radians(a);
  m = [cos(a) -sin(a) 0 0; ...
       sin(a) cos(a) 0 0; ...
       0 0 1 0; ...
       0 0 0 1];
endfunction

function cone(openingAngle, matrix=eye(4))
  halfAngle = radians(openingAngle * 0.5);
  disp("---- next-cone ----")
  disp("half_angle: "),disp(halfAngle);
  disp("tan(half_angle): "),disp(tan(halfAngle));
  disp("1/cos(half_angle): "),disp(1/cos(halfAngle));
  disp("direction: "),disp(matrix(1:3,3));
end

disp("######## 7-cone-bouquet ########")

angle = 60;
for i = [0:5]
  m = rotationMatrixZ(60*i) * rotationMatrixX(90-angle/2);
  cone(angle, m);
end

cone(angle);

disp("######## 10-cone-bouquet ########")

angle = 60;
for i = [0:5]
  m = rotationMatrixZ(60*i) * rotationMatrixX(90-angle/2);
  cone(angle, m);
end

angle = 45;
for i = [0:2]
  m = rotationMatrixZ(120*i) * rotationMatrixX(angle/2);
  cone(angle, m);
end
