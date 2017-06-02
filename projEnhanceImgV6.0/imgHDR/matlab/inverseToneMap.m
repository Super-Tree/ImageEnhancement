clear all;
Lmax=1.3;
Lwhite=1.3;
x=0:1:255;
Lw=0.5*Lmax*Lwhite*( (x-1) + sqrt((1-x).^2+4*x/Lwhite/Lwhite) );

plot(Lw,'r-');