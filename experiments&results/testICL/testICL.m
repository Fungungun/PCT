clear all; clc ;
videolist = dir('C:\Users\21558188\Desktop\PhD\Code\VS\covImageIntegral2\videos\');
videolist(1:2) = [];
output = 1;
frameNum = 80;
videoNum = 10;
for videocount = 1:videoNum
    video = videolist(videocount).name;
    %video = 'girl2';
    path = ['C:\Users\21558188\Desktop\PhD\Code\VS\covImageIntegral2\videos\' video];
    file  = dir(path);
    
    for i = 3:size(file,1)-2
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
    
    %init model1
    model1 = logm(cov(featmat_init));
    
    %init model2
    NPixelsPre = size(featmat_init,1);
    CovHatPre  = cov(featmat_init);
    uHatPre    = sum(featmat_init)/NPixelsPre;
    wHatPre    = NPixelsPre;
    wBarSquPre = 1/NPixelsPre;
    k = 0.95;
    model2 = logm(CovHatPre);
    
    
    model_library{1} = model1;
    feat_library{1} = featmat_init;
    sizefeat = size(featmat_init);
    
    for i = 2:10
        feat_library{i}  = randn(sizefeat)*0.01+featmat_init;
        model_library{i} = logm(cov(feat_library{i}));
        %         model_library{i} = model1;
        %         feat_library{i} = featmat_init;
    end
    
    featmat_icl = featmat_init;
    
    %for i = 2:size(filename,2)-1
    for i = 2:frameNum
        img = imread(filename{i});
        currentPos  =  pos_gt(i,:);
        currentArea =  img(currentPos(2):currentPos(4),currentPos(1):currentPos(3),:);
        rawdata = FeatureMatrix(currentArea);
        featmat = reshape(rawdata,[],17);
        currentModel = logm(cov(featmat));
        
        NPixelsCur = size(featmat,1);
        CovCur     = cov(featmat);
        uCur       = sum(featmat)/NPixelsCur;
        
        
        %         current_Npixels    = size(featmat,1);
        %         current_sumFeatmat = sum(featmat);
        %         current_IIprod     = featmat' * featmat;
        %         current_IIsum      = current_sumFeatmat'*current_sumFeatmat;
        
        %---compute the average covariance matrix
        dist1(i) = norm(model1 - currentModel,'fro');
        %         dist1(i) = norm(model1 - currentModel,'fro');
        %         for j = 2:9
        %             model_library{j} = model_library{j+1};
        %         end
        %         model_library{10} = currentModel;
        %         sumModel = model_library{1};
        %         for j = 2:10
        %             sumModel = sumModel + model_library{j};
        %         end
        %         model1 = sumModel/10;
        
        %---compute the individual distance between candidate with each
        %template
        for j = 1:10
            distT(j,i) = norm(model_library{j} - currentModel,'fro');
        end
        
        %---update model library
        distT(:,i);
        [maxDis,maxDisIndex] = max(distT(:,i));
        if maxDisIndex == 1
            [~,secondMaxDisIndex] = max(distT((distT(:,i)<maxDis),i));
            model_library{secondMaxDisIndex} = currentModel;
        else 
            model_library{maxDisIndex} = currentModel;
        end
        
        %---update model1
        sumModel = model_library{1};
        for j = 2:10
            sumModel = sumModel + model_library{j};
        end
        model1 = sumModel/10;
        
        %---compute distance for incremental learning model
        dist2(i) = norm(model2 - currentModel,'fro');
        %         dist2(i) = norm(model2 - currentModel,'fro');
        %         for j =2:9
        %             feat_library{j} = feat_library{j+1};
        %         end
        %         feat_library{10} = featmat;
        %         featmat_icl = feat_library{1};
        %         for j = 2:10
        %             featmat_icl = [featmat_icl;feat_library{j}];
        %         end
        %         model2 = logm(cov(featmat_icl));
        
        %---update model2
        wHatCur     = k*wHatPre + NPixelsCur;
        wBarSquCur  = (((wHatPre*wBarSquPre)^2 - NPixelsPre)*(k^2) + NPixelsCur) / (wHatCur^2);
        uHatCur     = (k*wHatPre*uHatPre + NPixelsCur*uCur)/wHatCur;
        CovHatCur   = (k*wHatPre*(1-wBarSquPre)*CovHatPre ...
            + (NPixelsCur-1)*CovCur ...
            + k*wHatPre*NPixelsCur/wHatCur*(uCur-uHatPre)*(uCur-uHatPre)')/(wHatCur*(1-wBarSquCur^2));
        
        wHatPre    = wHatCur;
        wBarSquPre = wBarSquCur;
        uHatPre    = uHatCur;
        CovHatPre  = CovHatCur;
        model2 = logm(CovHatPre);
    end
    figure
    plot(dist1)
    hold on
    plot(dist2)
    legend('Average library covmat','Increnmental learning')
    plot(distT(1,:),'ro');
    for i = 2:10
        plot(distT(i,:),'b*');
    end
    
    
    title(video)
    xlabel('frame number(starts from frame 2)')
    ylabel('distance')
    xlim([2 frameNum])
    grid on
    
    if(output)
        saveas(gcf,[video '.jpg'])
        close all
    end
end