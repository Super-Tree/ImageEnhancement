close all
clc
rgb=imread('D:\git_repository\Project\MATLAB\PROJ1\data\fogPic.jpg');
% 对饱和度与亮度进行histeq处理
hsv=rgb2hsv(rgb);
h=hsv(:,:,1);
s=hsv(:,:,2);
v=hsv(:,:,3);
S=histeq(s);
V=histeq(v);
result_hsv=hsv2rgb(h,S,V);

% 对RGB每个通道进行histeq处理
r=rgb(:,:,1);
g=rgb(:,:,2);
b=rgb(:,:,3);
R=histeq(r);
G=histeq(g);
B=histeq(b);
result_rgb=cat(3,R,G,B);

% 对YCbCr的亮度进行histeq处理
ycbcr=rgb2ycbcr(rgb);
y=ycbcr(:,:,1);
cb=ycbcr(:,:,2);
cr=ycbcr(:,:,3);
Y=histeq(y);
result_ycbcr=ycbcr2rgb(cat(3,Y,cb,cr));

% 结果显示
figure
subplot(2,2,1),imshow(rgb),title('原始图像')
subplot(2,2,2),imshow(result_hsv),title('HSV')
subplot(2,2,3),imshow(result_rgb),title('RGB')
subplot(2,2,4),imshow(result_ycbcr),title('YCbCr')
