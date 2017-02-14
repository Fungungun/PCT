%% illumination change on covariance matrix

%% Without normalization
%crop img
img = imread('feature.jpg');
roi = rgb2gray(img(214:295,198:232,:));

%compute the covariance matrix
roifeature = FeatureMatrix(roi);
roifeature_vector = reshape(roifeature,[],7);
covmat = cov(roifeature_vector);


%scale the img 
roi1 = round(1.2 * roi);

%compute the covariance matrix 
roifeature1 = FeatureMatrix(roi1);
roifeature_vector1 = reshape(roifeature1,[],7);
covmat1 = cov(roifeature_vector1);

format short
disp('scale is 1.2')
disp('for whole matrix: ')
covmat1./covmat
disp('for diagonal elements: ')
diag(covmat1)./diag(covmat)
disp('for trace: ')
trace(covmat1)/trace(covmat)

%scale the img
roi2 = round(0.9 * roi);

%compute the covariance matrix 
roifeature2 = FeatureMatrix(roi2);
roifeature_vector2 = reshape(roifeature2,[],7);
covmat2 = cov(roifeature_vector2);

disp('scale is 0.9')
disp('for whole matrix: ')
covmat2./covmat
disp('for diagonal elements: ')
diag(covmat2)./diag(covmat)
disp('for trace: ')
trace(covmat2)/trace(covmat)

%% Normalization applied

roi = rgb2gray(img(214:295,198:232,:));
roifeature = FeatureMatrix(roi);
roifeature_vector = reshape(roifeature,[],7);
roifeature_vector = roifeature_vector./repmat(sqrt(sum(roifeature_vector,1)),size(roifeature_vector,1),1);
covmat = cov(roifeature_vector);

%scale the img 
roi1 = round(1.2 * roi);

roifeature1 = FeatureMatrix(roi1);
roifeature_vector1 = reshape(roifeature1,[],7);
roifeature_vector1 = roifeature_vector1./repmat(sqrt(sum(roifeature_vector1,1)),size(roifeature_vector1,1),1);
covmat1 = cov(roifeature_vector1);

format short
disp('scale is 1.2')
disp('for whole matrix: ')
covmat1./covmat
disp('for diagonal elements: ')
diag(covmat1)./diag(covmat)
disp('for trace: ')
trace(covmat1)/trace(covmat)

%scale the img 
roi1 = round(0.9 * roi);

roifeature2 = FeatureMatrix(roi2);
roifeature_vector2 = reshape(roifeature2,[],7);
roifeature_vector2 = roifeature_vector2./repmat(sqrt(sum(roifeature_vector2,1)),size(roifeature_vector2,1),1);
covmat2 = cov(roifeature_vector2);

format short
disp('scale is 0.9')
disp('for whole matrix: ')
covmat2./covmat
disp('for diagonal elements: ')
diag(covmat2)./diag(covmat)
disp('for trace: ')
trace(covmat2)/trace(covmat)


%% conclusion 
% normalization works
