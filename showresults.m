function showresults(file)

clc
warning('off')
%-------------------------load data
pf = fopen([file '_results.txt'],'r');
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
numFrame = length(num);
s_frames = cell(numFrame,1);
nz	= strcat('%0',num2str(numzeros),'d');
fext = 'jpg';
for t=1:numFrame
    id=sprintf(nz,num(t));
    s_frames{t} = strcat('./',file,'/',id,'.',fext);
    point_draw = calcpoint_draw(pos(t,:));
    imshow(s_frames{t}) 
    hold on
    index = mode9_index(mode(t),:);
    text(20,20,['mode: ' num2str(mode(t))],'FontSize',14,'Color','red')
    for i = 1:6
        x = [point_draw(index(i),1),point_draw(index(i+1),1)];
        y = [point_draw(index(i),2),point_draw(index(i+1),2)];
        plot(x,y,'r-')
    end
   pause(0.01)
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





