from __future__ import division  
def get_overlap_area(box_a, box_b):
    x1_a, y1_a, width_a, height_a = box_a
    x1_b, y1_b, width_b, height_b = box_b

    x2_a = x1_a + width_a
    y2_a = y1_a + height_a
    x2_b = x1_b + width_b
    y2_b = y1_b + height_b

    #get the width and height of overlap rectangle
    overlap_width =  min(x2_a, x2_b) - max(x1_a, x1_b) 
    overlap_height = min(y2_a, y2_b) - max(y1_a, y1_b) 

    #If the width or height of overlap rectangle is negative, it implies that two rectangles does not overlap.
    if overlap_width > 0 and overlap_height > 0:
        return overlap_width * overlap_height
    else:
        return 0
  

def get_IOU(box_a, box_b):
    overlap_area = get_overlap_area(box_a, box_b)
    
    #Union = A + B - I(A&B)
    area_a = box_a[2] * box_a[3]
    area_b = box_b[2] * box_b[3]
    union_area = area_a + area_b - overlap_area
    
    
    if overlap_area > 0 :
        return  overlap_area / union_area
    else:
        return 0




pos_res = [50,50,100,100]
pos_gt  = [100,100,100,100]
score = get_IOU(pos_res,pos_gt)
print score

