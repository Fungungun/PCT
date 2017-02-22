function RawDataMatrix = FeatureMatrix(img)

%imgSize = [rows(y) cols(x) channels]
[Nrows,Ncols,Nchannels] = size(img);
img = double(img);

%construct matrix to store all the raw data according to the num of channel of img
%feature vector of each pixel %fx = [x y E1 ... Ei ... En]
%Ei = [Ii Ixi Iyi Ixxi Iyyi]
%i is the index of channel = 1 .. N
%N = 1, d = 7 * 1 + 2 = 9
%N = 3, d = 5 * 3 + 2 = 17

RawDataMatrix = zeros(Nrows, Ncols, 2 + Nchannels*5);
[RawDataMatrix(:,:,1), RawDataMatrix(:,:,2)] = meshgrid(1:Ncols, 1:Nrows);

offset = 0;
for i = 1:Nchannels
    RawDataMatrix(:,:,3+offset) = img(:,:,i); 
 
    RawDataMatrix(:,:,4+offset) = gradx(img(:,:,i)); 
    RawDataMatrix(:,:,5+offset) = grady(img(:,:,i)); 
        
    RawDataMatrix(:,:,6+offset) = grad2x(img(:,:,i));
    RawDataMatrix(:,:,7+offset) = grad2y(img(:,:,i)); 
    
    offset = offset + 5;
end
end

% -----------------------------------------

function Ix = gradx(I)
Ix = zeros(size(I));
Ix(:,2:end-1) = I(:,3:end) - I(:,1:end-2);
Ix(:,1) = Ix(:,2);
Ix(:,end) = Ix(:,end-1);
Ix = Ix/2;
end

function Iy = grady(I)
Iy = gradx(I')';
end

function Ixx = grad2x(I)
Ixx = zeros(size(I));
Ixx(:,2:end-1) = I(:,3:end) - 2*I(:,2:end-1) + I(:,1:end-2);
Ixx(:,1) = Ixx(:,2);
Ixx(:,end) = Ixx(:,end-1);
end

function Iyy = grad2y(I)
Iyy = grad2x(I')';
end
   

