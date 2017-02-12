function PatchCov = imgCov(pos,TargetFea)
%This function return the covariance matrics in nine modes.

[Ny Nx Nfea] = size(TargetFea);
[masks wt] = createMask9(Nx,Ny);

posx = pos(2,:);
posy = pos(1,:);
xbar = mean(posx);
ybar = mean(posy);

k = cell(Nfea,1);
[k{1},k{2}] = meshgrid((posx(1):posx(2))-xbar, ...
    (posy(1):posy(2))-ybar);
for i = 3:Nfea
    k{i} = TargetFea(:,:,i);
end

feat = cell(9,1);
PatchCov = cell(9,1);

for i=1:9
    mii = masks{i};
    for ii = 1:Nfea
        c = k{ii}(mii);
        eval(['feat{i} =[feat{i} c(:)];']);
    end 
    PatchCov{i} = cov(feat{i});  
end
end