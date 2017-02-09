function [M,wt] = createMask9(Nx, Ny)

% createMask9 creates a cell array containing 9 masks which can be
% used for masking out the part of a 2D region to be excluded from
% further computation. Each mask is a 2D logical array containing 1 or
% 0.
% 
% Input parameters:
%   Nx - number of elements in the x-direction (or number of columns)
%   Ny - number of elements in the y-direction (or number of rows)
% Output parameters:
%   M - a cell array of 9 elements for the 9 modes, detailed below:
%       Mode 1: the entire region is used (containing no 0s).
%       Mode 2: the top-left quadrant contains 0s.
%       Mode 3: the top-right quadrant contains 0s.
%       Mode 4: the bottom-left quadrant contains 0s.
%       Mode 5: the bottom-right quadrant contains 0s.
%       Mode 6: the left half contains 0s.
%       Mode 7: the right half contains 0s.
%       Mode 8: the top half contains 0s.
%       Mode 9: the bottom half contains 0s.
%   wt - an array containing the 9 weighting factors that can be
%        applied for the 9 modes above. Each weighting factor is
%        inversely proportional to the number of pixels covered by the
%        corresponding mode, e.g., mode 1 covers the entire region so
%        its weight is 1/4; modes 2-5 covers 3 quarters of the region
%        so its weight is 1/3; modes 6-9 covers half of the region so
%        its weight is 1/2.

M = cell(9,1);

Mfull = true(Ny, Nx);
% mode 1
M{1} = Mfull;
% mode 2
M3quarter = Mfull;
M3quarter(1:fix((1+Ny)/2), 1:fix((1+Nx)/2)) = 0;
M{2} = M3quarter;
% mode 3
M3quarter = Mfull;
M3quarter(1:fix((1+Ny)/2), (fix((1+Nx)/2)+1):end) = 0;
M{3} = M3quarter;
% mode 4
M3quarter = Mfull;
M3quarter((fix((1+Ny)/2)+1):end, 1:fix((1+Nx)/2)) = 0;
M{4} = M3quarter;
% mode 5
M3quarter = Mfull;
M3quarter((fix((1+Ny)/2)+1):end, (fix((1+Nx)/2)+1):end) = 0;
M{5} = M3quarter;
% mode 6
Mhalf = Mfull;
Mhalf(:, 1:fix((1+Nx)/2)) = 0;
M{6} = Mhalf;
% mode 7
Mhalf = Mfull;
Mhalf(:, (fix((1+Nx)/2)+1):end) = 0;
M{7} = Mhalf;
% model 8
Mhalf = Mfull;
Mhalf(1:fix((1+Ny)/2), :) = 0;
M{8} = Mhalf;
% mode 9
Mhalf = Mfull;
Mhalf((fix((1+Ny)/2)+1):end, :) = 0;
M{9} = Mhalf;

wt = [1/4; ones(4,1)/3; ones(4,1)/2];
end