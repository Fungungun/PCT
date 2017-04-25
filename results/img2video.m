function img2video(file,framerate)
imageNames= dir(fullfile(['./resultImg/' file],'*.jpg'));
imageNames = {imageNames.name}';
outputVideo = VideoWriter(fullfile('./',[file '.avi']));
outputVideo.FrameRate = framerate;
open(outputVideo)
for ii = 1:length(imageNames)
   img = imread(fullfile(['./resultImg/' file '/'],imageNames{ii}));
   writeVideo(outputVideo,img)
end
close(outputVideo)