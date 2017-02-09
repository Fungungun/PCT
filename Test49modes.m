clc

roi = featimage(15:95,10:90,:);
[Ny Nx Nfea] = size(roi);
[masks wt] = createMask9(Nx,Ny);

for i = 1:Nfea
    k{i} = roi(:,:,i);
end
    
feat = cell(9,1);
PatchCov = cell(9,1);

for i=1:9
    mii = masks{i};
    for ii = 1:Nfea
        c = k{ii}(mii);
        feat{i} =[feat{i} c(:)];
    end 
     PatchCov{i} = cov(feat{i});  
end

for i = 1:9
    disp(['Covariance matrix for mode ' num2str(i) ' is:'])
    covmat_m(:,:,i) = PatchCov{i}
end


distance = covmat_m - covmat_c;
percentage = distance ./ covmat_c
max(max(percentage))