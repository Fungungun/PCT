clc;clear all
run test_metric_dis_data
format bank
for i = 1 : 600
    par_dis_m(i) = norm(canpar_model(:,:,i) - tarpar_model,'fro');
end
par_dis_m - par_dis(:,1)'

box_gt  = [pos_gt(1) pos_gt(2) pos_gt(3)-pos_gt(1) pos_gt(4)-pos_gt(2)];
box_par = [par_pos(:,1) par_pos(:,2) par_pos(:,3)-par_pos(:,1) par_pos(:,4)-par_pos(:,2)];
for i=1:600
    iou(i) = bboxOverlapRatio(box_gt,box_par(i,:));
end
k = [par_dis_m',iou]



gt_x = (pos_gt(1) + pos_gt(3))/2;
gt_y = (pos_gt(2) + pos_gt(4))/2;

par_x = (par_pos(:,1) + par_pos(:,3))/2;
par_y = (par_pos(:,2) + par_pos(:,4))/2;

cendis = sqrt(((par_x - gt_x).^2 + (par_y - gt_y).^2));
k = [par_dis_m',cendis];

min_dis
min_index
cendis(min_index(1))

[mindis minindex] = min(par_dis);

%conclusion: minimum distance is not a good option as it will be influenced
%by noise 

