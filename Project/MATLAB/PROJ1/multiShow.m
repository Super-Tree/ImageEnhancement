clc;
cd('D:\git_repository\Project\MATLAB\PROJ1\data');
allnames=struct2cell(dir('*.jpg')); %ֻ����jpg�ļ�
[k,len]=size(allnames); %���jpg�ļ��ĸ���
for ii=1:len     
%���ȡ���ļ�
name=allnames{1,ii};
D=imread(name); %��ȡ�ļ�
%Ȼ���ڴ˴�������ͼ������򼴿�
A = im2double(D);
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
%I = (r + g + b)/3;
I=r*0.299 + g*0.587 + b*0.114;
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
%[X,Y] = meshgrid(1242:-1:1,1:1:375);
[X,Y] = meshgrid(640:-1:1,1:1:480);
mesh(X,Y,I);
C=rgb2gray(D);
figure(3)
imhist(C);
pause();
end