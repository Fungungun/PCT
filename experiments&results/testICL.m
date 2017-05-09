clear all; clc ;
videolist = dir('C:\Users\21558188\Desktop\PhD\Code\VS\covImageIntegral2\videos\');
videolist(1:2) = [];

for videocount = 1:10
    video = videolist(videocount).name;
   % video = 'ball';
    path = ['C:\Users\21558188\Desktop\PhD\Code\VS\covImageIntegral2\videos\' video];
    file  = dir(path);
    
    for i = 3:size(file,1)-2
    %for i = 3:50
        filename{i-2} = [path '\' file(i).name];
    end
    filename{end+1} = [path '\gt.txt'];
    %filename(end)
    % ground truth in 1st frame: [x1 y1 width height] = [200 113 45 45]
    
    pos_gt = importdata(filename{end});
    pos_gt(:,3) = pos_gt(:,1) + pos_gt(:,3);
    pos_gt(:,4) = pos_gt(:,2) + pos_gt(:,4);
    
    pos_gt = floor(pos_gt);
    init_pos = pos_gt(1,:);
    
    img = imread(filename{1});
    %imshow(img)
    target = img(init_pos(2):init_pos(4),init_pos(1):init_pos(3),:);
    %imshow(target)
    
    rawdata_init = FeatureMatrix(target);
    featmat_init = reshape(rawdata_init,[],17);
    model1 = logm(cov(featmat_init));
    model2 = model1;
    
    model_library{1} = model1;
    for i = 2:10
        % model_library{i} = model_library{1} + 2*randn(size(model_library{1}));
        model_library{i} = model_library{1};
    end
    
    
    featmat_icl = featmat_init;
    dist1(1) = 0;
    dist2(2) = 0;
    %for i = 2:size(filename,2)-1
    for i = 2:100
        img = imread(filename{i});
        currentPos  =  pos_gt(i,:);
        currentArea =  img(currentPos(2):currentPos(4),currentPos(1):currentPos(3),:);
        rawdata = FeatureMatrix(currentArea);
        featmat = reshape(rawdata,[],17);
        currentModel = logm(cov(featmat));
        
        % compute the average covariance matrix
        dist1(i) = norm(model1 - currentModel,'fro');
        
        for j = 2:9
            model_library{j} = model_library{j+1};
        end
        model_library{10} = currentModel;
        sumModel = model_library{1};
        for k = 2:10
            sumModel = sumModel + model_library{k};
        end
        model1 = sumModel/10;
        
        % compute incrementally learning cov mat
        dist2(i) = norm(model2 - currentModel,'fro');
        featmat_icl = [featmat_icl;featmat];
        model2 = logm(cov(featmat_icl));
    end
    figure
    plot(dist1)
    hold on
    plot(dist2)
    title(video)
    legend('average covmat','Increnmental learning')
    xlabel('frmae number')
    ylabel('distance')
    saveas(gcf,[video '.jpg'])
    close all
end