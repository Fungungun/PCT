run test_normprob
format bank
par_prob_m = 1./par_dis;
sum_prob_m = sum(par_prob_m);
sizeM = size(par_prob_m);
nParticles = sizeM(1);
par_prob_m = par_prob_m ./ repmat(sum_prob_m,[nParticles 1]);
par_prob_m - par_prob