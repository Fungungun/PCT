
a = randn(10);
IIa1 = integralImage(a);

IIa1(7,6)+IIa1(6,5)-IIa1(6,6)-IIa1(7,5)

a([1:4,10],:) = 0; 
a(:,[1:3,8:10]) = 0;
a;
IIa2 = integralImage(a);
IIa2(7,6)+IIa2(6,5)-IIa2(6,6)-IIa2(7,5)