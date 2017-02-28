i = 1;

video_sequence{i}.name = 'girl2'; video_sequence{i}.fps = 50; video_sequence{i}.wndsize = 400;  i = i+1;
video_sequence{i}.name = 'Walking'; video_sequence{i}.fps = 50; video_sequence{i}.wndsize = 400;  i = i+1;
video_sequence{i}.name = 'surfing'; video_sequence{i}.fps = 50; video_sequence{i}.wndsize = 400;  i = i+1;
video_sequence{i}.name = 'David3'; video_sequence{i}.fps = 50; video_sequence{i}.wndsize = 200;  i = i+1;
video_sequence{i}.name = 'bolt'; video_sequence{i}.fps = 50; video_sequence{i}.wndsize = 400;  i = i+1;
video_sequence{i}.name = 'FaceOcc1'; video_sequence{i}.fps = 50; video_sequence{i}.wndsize = 400;  i = i+1;
video_sequence{i}.name = 'polarbear'; video_sequence{i}.fps = 50; video_sequence{i}.wndsize = 400; i = i+1;
video_sequence{i}.name = 'bicycle'; video_sequence{i}.fps = 50; video_sequence{i}.wndsize = 400;  i = i+1;
video_sequence{i}.name = 'ball'; video_sequence{i}.fps = 50; video_sequence{i}.wndsize = 400;  i = i+1;
video_sequence{i}.name = 'sphere'; video_sequence{i}.fps = 50; video_sequence{i}.wndsize = 400;  i = i+1;
video_sequence{i}.name = 'jogging'; video_sequence{i}.fps = 50; video_sequence{i}.wndsize = 400;  i = i+1;
video_sequence{i}.name = 'jogging2'; video_sequence{i}.fps = 50; video_sequence{i}.wndsize = 400;  i = i+1;

results.numFrame = 0;
results.sumiou   = 0;

for i = 1:(length(video_sequence)-1)
    tmp = showresults(video_sequence{i}.name,video_sequence{i}.fps,video_sequence{i}.wndsize);
    results.numFrame = results.numFrame + tmp.numFrame;
    results.sumiou   = results.sumiou + tmp.sumiou;
end

hs = msgbox(['Average IOU score is ' num2str(results.sumiou/results.numFrame)]);
ht = findobj(hs, 'Type', 'text');
set(ht, 'FontSize',50, 'Unit', 'normal');
set(hs, 'Position', [300 300 1000 300]); 

