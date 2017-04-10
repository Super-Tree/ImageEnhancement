close all
clc
rgb=imread('D:\git_repository\Project\MATLAB\PROJ1\data\fogPic.jpg');
% �Ա��Ͷ������Ƚ���histeq����
hsv=rgb2hsv(rgb);
h=hsv(:,:,1);
s=hsv(:,:,2);
v=hsv(:,:,3);
S=histeq(s);
V=histeq(v);
result_hsv=hsv2rgb(h,S,V);

% ��RGBÿ��ͨ������histeq����
r=rgb(:,:,1);
g=rgb(:,:,2);
b=rgb(:,:,3);
R=histeq(r);
G=histeq(g);
B=histeq(b);
result_rgb=cat(3,R,G,B);

% ��YCbCr�����Ƚ���histeq����
ycbcr=rgb2ycbcr(rgb);
y=ycbcr(:,:,1);
cb=ycbcr(:,:,2);
cr=ycbcr(:,:,3);
Y=histeq(y);
result_ycbcr=ycbcr2rgb(cat(3,Y,cb,cr));

% �����ʾ
figure
subplot(2,2,1),imshow(rgb),title('ԭʼͼ��')
subplot(2,2,2),imshow(result_hsv),title('HSV')
subplot(2,2,3),imshow(result_rgb),title('RGB')
subplot(2,2,4),imshow(result_ycbcr),title('YCbCr')
