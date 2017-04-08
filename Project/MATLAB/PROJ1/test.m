clc;
Q=imread('./data/pic2.jpg');
e = Q(:, :, 1);
K1=filter2(fspecial('average',13),e)/255;
    medfilt2(e,[100 100]);
figure(1)
imshow(K1),title('origin2');
pause;