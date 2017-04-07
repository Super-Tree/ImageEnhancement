srcDir=uigetdir('Choose source directory.'); %获得选择的文件夹
cd(srcDir);
allnames=struct2cell(dir('*.jpg')); %只处理jpg文件
[k,len]=size(allnames); %获得jpg文件的个数
for ii=1:len
%逐次取出文件
name=allnames{1,ii};
A=imread(name); %读取文件
%然后在此处添加你的图像处理程序即可
medfilt2(A,[10 10]);
A = im2double(A);
r = A(:, :, 1);
g = A(:, :, 2);
b = A(:, :, 3);
figure(1)
subplot(2,2,1);imshow(A),title('origin');
subplot(2,2,2);imshow(r),title('1R');
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
pause();
end