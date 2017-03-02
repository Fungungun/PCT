function result = showresults(file,fps,wndsize,showimage)

%file   : ball, surfing, david3 etc
%fps    : 100,50,20 etc
%wndsize: 100:100%(original) 200:twice bigger

warning('off')
file
%-------------------------load data
pf = fopen([file '.txt'],'r');
tline = fgetl(pf);
str = strsplit(tline);
file  = str{1};
dataset = str2num(str{2});
i = 1;

while ~feof(pf)
    tline = fgetl(pf);
    str = strsplit(tline);
    num(i) = str2num(str{1});
    mode(i) = str2num(str{2});
    pos(i,1) = str2num(str{3}(2:end-1));
    
    pos(i,2) = str2num(str{4}(1:end-1));
    pos(i,3) = str2num(str{5}(1:end-1));
    pos(i,4) = str2num(str{6}(1:end-2));
    i = i + 1;
end
fclose(pf);
%-------------------------load groundtruth
pf = fopen(['../../videos/' file '/gt.txt'],'r');
tline = fgetl(pf); % get rid of the first line
i = 1;
str = strsplit(tline,',');
if length(str{1}) < length(tline)
    gt_delimiter = ',';
else
    gt_delimiter = char(9);
    str = strsplit(tline,gt_delimiter);
end

while ~feof(pf)
    tline = fgetl(pf);
    pos_gt(i,1) = str2num(str{1});
    pos_gt(i,2) = str2num(str{2});
    pos_gt(i,3) = str2num(str{3}) + str2num(str{1});
    pos_gt(i,4) = str2num(str{4}) + str2num(str{2});
    str = strsplit(tline,gt_delimiter);
    i = i + 1;
end
fclose(pf);
%--------------------------calculate iou
pos_wh    = [pos(:,1) pos(:,2) pos(:,3)-pos(:,1) pos(:,4)-pos(:,2)];
pos_gt_wh = [pos_gt(:,1) pos_gt(:,2) pos_gt(:,3)-pos_gt(:,1) pos_gt(:,4)-pos_gt(:,2)];
for i = 1:size(pos_wh,1)
    iou(i) = bboxOverlapRatio(pos_wh(i,:),pos_gt_wh(i,:));
end

numFrame = length(num);
result.numFrame = numFrame;
result.sumiou = sum(iou);
avgIOU = result.sumiou/result.numFrame

if showimage == 1
    %---------------------------show image
    % 1------2------3
    % |      |      |
    % 4------5------6
    % |      |      |
    % 7------8------9
    mode9_index = [
        1 3 9 7 1 3 9; %mode 1
        2 3 9 7 4 5 2; %mode 2
        1 2 5 6 9 7 1; %mode 3
        1 3 9 8 5 4 1; %mode 4
        1 3 6 5 8 7 1; %mode 5
        2 3 9 8 2 3 9; %mode 6
        1 2 8 7 1 2 8; %mode 7
        4 6 9 7 4 6 9; %mode 8
        1 3 6 4 1 3 6; %mode 9
        ];
    
    numzeros = (dataset == 0) * 4 + (dataset == 1) * 8;
    s_frames = cell(numFrame,1);
    nz	= strcat('%0',num2str(numzeros),'d');
    fext = 'jpg';
    for t=1:numFrame
        id=sprintf(nz,num(t));
        s_frames{t} = strcat('../../videos/',file,'/',id,'.',fext);
        point_draw = calcpoint_draw(pos(t,:));
        %image
        imshow(s_frames{t},'InitialMagnification',wndsize);
        hold on
        %groud truth
        rectangle('Position',pos_gt_wh(t,:),'LineWidth',10);
        %text
        index = mode9_index(mode(t),:);
        text(20,20,['frame ' num2str(t) '  IOU score ' num2str(iou(t)) '  mode ' num2str(mode(t))], ...
            'FontSize',14,'Color','red')
        for i = 1:6
            x = [point_draw(index(i),1),point_draw(index(i+1),1)];
            y = [point_draw(index(i),2),point_draw(index(i+1),2)];
            plot(x,y,'w-','LineWidth',10)
        end
        pause(1/fps)
        cla
    end
    close all
end

end



function point = calcpoint_draw(pos)
x1 = pos(1);y1 = pos(2);x2 = pos(3);y2 = pos(4);
xhalf = (x1+x2)/2; yhalf = (y1+y2)/2;
point = [
    x1 y1;      xhalf y1;       x2 y1;
    x1 yhalf;   xhalf yhalf;    x2 yhalf;
    x1 y2;      xhalf y2;       x2 y2;];
end





