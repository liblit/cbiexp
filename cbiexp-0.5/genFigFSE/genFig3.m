data = load('results1.txt');
N = max(data(:,2));
if mod(size(data,1),N) ~= 0, error 'wrong dimension'; end;
M = size(data,1);
data(:,1:2) = [];
retiled = [];
ctr = 1;
while ctr < M,
  retiled = [retiled data(ctr:ctr+N-1,:)];
  ctr = ctr + N;
end;
data1 = retiled;

data = load('results10.txt');
N = max(data(:,2));
if mod(size(data,1),N) ~= 0, error 'wrong dimension'; end;
M = size(data,1);
data(:,1:2) = [];
retiled = [];
ctr = 1;
while ctr < M,
  retiled = [retiled data(ctr:ctr+N-1,:)];
  ctr = ctr + N;
end;
data2 = retiled;

data = load('results100.txt');
N = max(data(:,2));
if mod(size(data,1),N) ~= 0, error 'wrong dimension'; end;
M = size(data,1);
data(:,1:2) = [];
retiled = [];
ctr = 1;
while ctr < M,
  retiled = [retiled data(ctr:ctr+N-1,:)];
  ctr = ctr + N;
end;
data3 = retiled;

data = load('results1000.txt');
N = max(data(:,2));
if mod(size(data,1),N) ~= 0, error 'wrong dimension'; end;
M = size(data,1);
data(:,1:2) = [];
retiled = [];
ctr = 1;
while ctr < M,
  retiled = [retiled data(ctr:ctr+N-1,:)];
  ctr = ctr + N;
end;
data4 = retiled;

t = [3000:3000:31875 31875];
brret1 = data1(:,1:3:end) + data1(:,2:3:end);
all1 = data1(:,1:3:end) + data1(:,2:3:end) + data1(:,3:3:end);
brret2 = data2(:,1:3:end) + data2(:,2:3:end);
all2 = data2(:,1:3:end) + data2(:,2:3:end) + data2(:,3:3:end);
brret3 = data3(:,1:3:end) + data3(:,2:3:end);
all3 = data3(:,1:3:end) + data3(:,2:3:end) + data3(:,3:3:end);
brret4 = data4(:,1:3:end) + data4(:,2:3:end);
all4 = data4(:,1:3:end) + data4(:,2:3:end) + data4(:,3:3:end);
figure(1); clf
h1 = errorbar(t,mean(all1,2),std(all1,0,2));
hold on;
h2 = errorbar(t,mean(all2,2),std(all2,0,2),'g-.');
h3 = errorbar(t,mean(all3,2),std(all3,0,2),'m--');
h4 = errorbar(t,mean(all4,2),std(all4,0,2),'r:');
xlabel('number of runs used','fontsize',14); ylabel('number of predicates retained','fontsize', 14);
legend([h1(2) h2(2) h3(2) h4(2)], 'Unsampled', 'sampling rate 1/10', 'sampling rate 1/100', 'sampling rate 1/1000', 4);
axis fill;
drawnow
print -f1 -depsc predkepta.eps

figure(2);clf;
h1 = errorbar(t,mean(brret1,2),std(brret1,0,2));
hold on;
h2 = errorbar(t,mean(brret2,2),std(brret2,0,2),'g-.');
h3 = errorbar(t,mean(brret3,2),std(brret3,0,2),'m--');
h4 = errorbar(t,mean(brret4,2),std(brret4,0,2),'r:');
xlabel('number of runs used', 'fontsize', 14); ylabel('number of predicates retained', 'fontsize', 14);
legend([h1(2) h2(2) h3(2) h4(2)], 'unsampled', 'samp rate 1/10', 'samp rate 1/100', 'samp rate 1/1000', 4);
axis fill;
drawnow
print -f2 -depsc predkeptb.eps

