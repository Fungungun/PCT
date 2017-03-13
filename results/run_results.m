clc
clear all
showimage = 0;

video = dir('C:\Users\21558188\Desktop\PhD\Code\VS\covImageIntegral2\videos');

results.numFrame = 0;
results.sumiou   = 0;
results.pos      = zeros(1,4);

for j = 1:length(video)-2
    tmp = showresults(video(j+2).name,50,400,showimage);
    results.numFrame = results.numFrame + tmp.numFrame;
    results.sumiou   = results.sumiou + tmp.sumiou;
    results.pos      = [results.pos; tmp.pos];
end
results.pos([1],:) = []; 
disp(['average IOU score of these ' num2str(length(video)-2) ' videos is'])
results.sumiou/results.numFrame

if showimage == 1
    hs = msgbox(['Average IOU score is ' num2str(results.sumiou/results.numFrame)]);
    ht = findobj(hs, 'Type', 'text');
    set(ht, 'FontSize',50, 'Unit', 'normal');
    set(hs, 'Position', [300 300 1000 300]);
end
