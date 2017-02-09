function RawDataMatrix = FeatureMatrix(img)

%imgSize = [rows(y) cols(x) channels]
[Nrows,Ncols,Nchannels] = size(img);
img = double(img);

%construct matrix to store all the raw data according to the num of channel of img
%feature vector of each pixel %fx = [x y E1 ... Ei ... En]
%Ei = [Ii |Ixi| |Iyi| |Ixxi| |Iyyi|]
%i is the index of channel = 1 .. N
%N = 1, d = 7 * 1 + 2 = 9
%N = 3, d = 5 * 3 + 2 = 17

RawDataMatrix = zeros(Nrows, Ncols, 2 + Nchannels*5);

[RawDataMatrix(:,:,1), RawDataMatrix(:,:,2)] = meshgrid(1:Ncols, 1:Nrows);

offset = 0;
for i = 1:Nchannels
    RawDataMatrix(:,:,3+offset) = img(:,:,i); %Ii
        
    [Ix, Iy] = gradient(img(:,:,i));
    RawDataMatrix(:,:,4+offset) = abs(Ix); %|Ixi|
    RawDataMatrix(:,:,5+offset) = abs(Iy); %|Iyi|
    
    [Ixx, ~] =  gradient(Ix);
    RawDataMatrix(:,:,6+offset) = abs(Ixx); %|Ixxi|
    [~, Iyy] =  gradient(Iy);
    RawDataMatrix(:,:,7+offset) = abs(Iyy); %|Iyyi|
    
    offset = offset + 5;
end


