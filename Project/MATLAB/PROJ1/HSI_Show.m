clc;
A=imread('./data/pic2.jpg');
A = im2double(A);
r = A(:, :, 1);
g = A(:, :, 2);
b = A(:, :, 3);
figure(1)
subplot(2,2,1);imshow(A),title('origin');
subplot(2,2,2);imshow(r),title('R');
subplot(2,2,3);imshow(g),title('G');
subplot(2,2,4);imshow(b),title('B');
 
 % Implement the conversion equations.
num = 0.5*((r - g) + (r - b));
den = sqrt((r - g).^2 + (r - b).*(g - b));
theta = acos(num./(den + eps));
H = theta;
H(b > g) = 2*pi - H(b > g);
H = H/(2*pi);
num = min(min(r, g), b);
den = r + g + b;
den(den == 0) = eps;
S = 1 - 3.* num./den;
H(S == 0) = 0;
I = (r + g + b)/3;
% Combine all three results into an hsi image.
hsi = cat(3, H, S, I);
 figure(2)
% subplot(2,2,1);imshow(hsi),title('origin');
% subplot(2,2,2);imshow(H),title('H');
% subplot(2,2,3);imshow(S),title('S');
% subplot(2,2,4);imshow(I),title('I');
[X,Y] = meshgrid(1:1:640,1:1:480);
mesh(X,Y,I);

