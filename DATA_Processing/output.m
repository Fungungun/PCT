clc;clear all;
%%
load('my_pos')
video = dir('C:\Users\21558188\Desktop\PhD\Code\VS\covImageIntegral2\videos');
%% load ground truth
pos_gt = zeros(1,4);
staple_pos = zeros(1,4);
for i = 1:length(video)-2
    %-----------------ground truth
    tmp = dlmread(['C:\Users\21558188\Desktop\PhD\Code\VS\covImageIntegral2\videos\' video(i+2).name '\gt.txt']);
    tmp([1],:) = [];%1st frame
    pos_gt = [pos_gt;tmp];
    
    %------------------staple results
    tmp = dlmread(['C:\Users\21558188\Desktop\PhD\GitHub\staple\results\' video(i+2).name '.txt'],'\t');
    tmp([1],:) = []; %1st frame
    staple_pos = [staple_pos;tmp];
    
end
pos_gt([1],:) = [];
staple_pos([1],:) = [];

numFrames = size(pos_gt,1);
%% metrics 
for i = 1:numFrames
    iou.my(i) = bboxOverlapRatio(my_pos(i,:),pos_gt(i,:));
    iou.staple(i) = bboxOverlapRatio(staple_pos(i,:),pos_gt(i,:));
    
    centre.my(i) = pdist([my_pos(i,1)+my_pos(i,3)/2 my_pos(i,2)+my_pos(i,4)/2;pos_gt(i,1)+pos_gt(i,3)/2 pos_gt(i,2)+pos_gt(i,4)/2;]);
    centre.staple(i) = pdist([staple_pos(i,1)+staple_pos(i,3)/2 staple_pos(i,2)+staple_pos(i,4)/2;pos_gt(i,1)+pos_gt(i,3)/2 pos_gt(i,2)+pos_gt(i,4)/2;]);
end

%% OPE overlap 
overlapX = 0:0.01:1;
for i = 1:length(overlapX)
    successIOU.my(i) = length(iou.my((iou.my>=overlapX(i))))/numFrames;
    successIOU.staple(i) = length(iou.staple((iou.staple>=overlapX(i))))/numFrames;
end
successIOUmean.my = mean(successIOU.my);
successIOUmean.staple = mean(successIOU.staple);
IOverlapOPE = figure;
plot(overlapX,successIOU.my,'r','linewidth',3)
hold on
plot(overlapX,successIOU.staple,'b','linewidth',3)
title('Success plots of OPE','FontSize',14,'FontWeight','bold');xlabel('Overlap Threshold','FontSize',14,'FontWeight','bold');ylabel('Success Rate','FontSize',14,'FontWeight','bold');

legend(['Our Method(' num2str(successIOUmean.my) ')'], ...
['Staple(' num2str(successIOUmean.staple) ')'],'FontSize',14,'FontWeight','bold');
axis normal;
box on
set(gca,'linewidth',3,'FontWeight','bold');
saveas(IOverlapOPE,'OverlapOPE','png')


%% OPE location
CentreX = 0:1:50;
for i = 1:length(CentreX)
    successCentre.my(i) = 1 - length(centre.my((centre.my>=CentreX(i))))/numFrames;
    successCentre.staple(i) = 1 - length(centre.staple((centre.staple>=CentreX(i))))/numFrames;
end
successCentremean.my = mean(successCentre.my);
successCentremean.staple = mean(successCentre.staple);
ILocationOPE = figure;
plot(CentreX,successCentre.my,'r','linewidth',3)
hold on
plot(CentreX,successCentre.staple,'b','linewidth',3)
title('Precision plots of OPE','FontSize',14,'FontWeight','bold');xlabel('Location error Threshold','FontSize',14,'FontWeight','bold');ylabel('Precision','FontSize',14,'FontWeight','bold');

legend(['Our Method(' num2str(successCentremean.my) ')'], ...
['Staple(' num2str(successCentremean.staple) ')'],'FontSize',14,'FontWeight','bold');
axis normal;
box on
set(gca,'linewidth',3,'FontWeight','bold');
saveas(ILocationOPE,'LocationOPE','png')

