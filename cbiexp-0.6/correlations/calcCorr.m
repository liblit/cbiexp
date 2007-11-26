Wf = full(Wf);
Ws = full(Ws);
Wfobs = full(Wfobs);
Wsobs = full(Wsobs);
% Wfcross(i,j) is number of times i is observed and j is true
Wfcross = full(Wfcross);  
Wscross = full(Wscross);
N = size(Wf,2);

%% P(A true | A obs, B obs)
PacondB = (Wfcross + Wscross)'./(Wfobs+Wsobs);
%PacondB = Wfcross'./Wfobs;
PacondB(isnan(PacondB)) = 0;
%% P(A and B true | A obs, B obs)
Pab = (Wf+Ws)./(Wfobs+Wsobs);
%Pab = Wf./Wfobs;
Pab(isnan(Pab)) = 0;
Varcond = PacondB .* (1- PacondB);
Cov = Pab - PacondB.*PacondB'; 
rho = Cov ./ (sqrt(Varcond.*Varcond'));
rho(isnan(rho)) = 0;

%% P(A true | B true) = P(A true, B true) / P(B true)
%PAB = (Wf+Ws)./repmat(diag(Wf+Ws)',N,1);

