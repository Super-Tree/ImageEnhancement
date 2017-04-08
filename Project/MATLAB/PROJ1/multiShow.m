clc;
cd('D:\git_repository\Project\MATLAB\PROJ1\data');
allnames=struct2cell(dir('*.png')); %只处理jpg文件
[k,len]=size(allnames); %获得jpg文件的个数
for ii=1:len     
%逐次取出文件
name=allnames{1,ii};
A=imread(name); %读取文件
%然后在此处添加你的图像处理程序即可

A = im2double(A);
r = A(:, :, 1);
g = A(:, :, 2);
b = A(:, :, 3);
figure(1)
% subplot(2,2,1);imshow(A),title('origin');
% subplot(2,2,2);imshow(r),title('1R');
% subplot(2,2,3);imshow(g),title('G');
% subplot(2,2,4);imshow(b),title('B');
 imshow(A),title('origin');
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
I=filter2(fspecial('average',13),I)/255;
% w1=fspecial('average',[15 15]);  
% g1=imfilter(I,w1,'replicate');  
% imshow(g1);
medfilt2(I,[40 40]);
% Combine all three results into an hsi image.
hsi = cat(3, H, S, I);
figure(2)
% subplot(2,2,1);imshow(hsi),title('origin');
% subplot(2,2,2);imshow(H),title('H');
% subplot(2,2,3);imshow(S),title('S');
% subplot(2,2,4);imshow(I),title('I');
[X,Y] = meshgrid(1:1:1242,1:1:375);
mesh(X,Y,I);
pause();
end