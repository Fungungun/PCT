
format bank
img = double(imread('../../videos/car4/0001.jpg'));
feamat = FeatureMatrix(img);
feamat_c(:,:,1) = xcor+1;
feamat_c(:,:,2) = ycor+1;
feamat_c(:,:,3) = I;
feamat_c(:,:,4) = Ix;
feamat_c(:,:,5) = Iy;
feamat_c(:,:,6) = Ixx;
feamat_c(:,:,7) = Iyy;

find(feamat - feamat_c)
target = feamat(51:138,70:177,:);
target = reshape(target,[],7);
disp('1')
cov(target)
tarpar_covmat
max((cov(target) - tarpar_covmat)./cov(target))
% disp('2')
% logm(cov(target)) - logm(tarpar_covmat)
% disp('3')
% (logm(cov(target)) - tarpar_model)
% disp('4')
% (logm(cov(target)) - logm(tarpar_covmat)) - ((logm(cov(target)) - tarpar_model))
