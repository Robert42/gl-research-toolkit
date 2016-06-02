module cone(openingAngle, length)
{
  translate([0, 0, length/2])
  cylinder(length, r1=0, r2=tan(openingAngle/2)*length,center=true);
}


module ConeArrangement6(l=10)
{

angle = 60;
l = 10;


cone(angle, l);

for(i=[0:5])
{
rotate([0, 0, 60*i])
rotate([90-angle/2, 0, 0])
cone(angle, l);
}

}


module ConeArrangement9(l=10)
{
angle1 = 45;
l1 = l;

for(i=[0:2])
{
rotate([0, 0, 120*i])
rotate([angle1/2, 0, 0])
cone(angle1, l1);
}

angle2 = 60;
l2 = l;

for(i=[0:5])
{
rotate([0, 0, 60*i])
rotate([90-angle2/2, 0, 0])
cone(angle2, l2);
}
}


translate([30, 0, 0])
ConeArrangement6();
ConeArrangement9();
